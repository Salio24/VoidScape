#include "Mover.hpp"

Mover::Mover(const Cori::Physics::Capsule& capsule, Cori::Physics::WorldRef world, Cori::Entity& player, const Params& def) : // GOD FUCKING LORD THATS A HUGE INIT-LIST
	m_JumpStartSpeed(def.jumpStartSpeed), m_JumpVariableSpeed(def.jumpVariableSpeed), m_JumpVariableTicks(def.jumpVariableTicks),
	m_JumpBufferTicks(def.jumpBufferTicks), m_JumpCoyoteTimeTicks(def.jumpCoyoteTimeTicks),
		m_WallJumpStartSpeed(def.wallJumpStartSpeed), m_WallJumpVariableSpeed(def.wallJumpVariableSpeed), m_WallJumpStartSideSpeed(def.wallJumpStartSideSpeed),
	m_WallJumpVariableSideSpeed(def.wallJumpVariableSideSpeed), m_WallJumpVariableTicks(def.wallJumpVariableTicks), m_WallJumpBufferTicks(def.wallJumpBufferTicks),
		m_DoubleJumpStartSpeed(def.doubleJumpStartSpeed), m_DoubleJumpVariableSpeed(def.doubleJumpVariableSpeed), m_DoubleJumpVariableTicks(def.doubleJumpVariableTicks),
	m_DoubleJumpRayModifierForRegular(def.doubleJumpRayModifierForRegular), m_DoubleJumpRayModifierForWall(def.doubleJumpRayModifierForWall), m_MaxSpeed(def.maxSpeed),
		m_MinSpeed(def.minSpeed), m_StopSpeed(def.stopSpeed),
	m_Acceleration(def.acceleration), m_AirSteer(def.airSteer), m_Friction(def.friction), m_GravityDefault(def.gravityDefault), m_FastFallGravityModifier(def.fastFallGravityModifier),
	m_PogoHertz(def.pogoHertz), m_PogoDampingRatio(def.pogoDampingRatio), m_PogoLengthScale(def.pogoLengthScale),
	m_SegmentOffset(def.segmentOffset), m_WallSlideSpeed(def.wallSlideSpeed), m_MinSpeedForRunState(def.minSpeedForRunState), m_World(world),
	m_Capsule(capsule), m_Player(player) {

	glm::vec2 spawn = player.GetComponents<Cori::Components::Entity::Spawnpoint>().m_Spawnpoint;

	m_Transform = { Cori::Physics::ToMeters(spawn), b2Rot_identity };

	//a hack to allow sensor use with an entity that is operated via mover
	Cori::Physics::Body::Params bp;
	bp.type = b2_kinematicBody;
	bp.position = m_Transform.p;
	bp.fixedRotation = true;
	bp.rotation = b2Rot_identity;

	auto& rb = m_Player.AddComponent<Cori::Components::Entity::Rigidbody>(world, bp, m_Player);

	Cori::Physics::Shape::Params sp;
	sp.filter.maskBits = Cori::Physics::CollisionBits::SensorBit;
	sp.enableSensorEvents = true;

	rb.CreateShape(Cori::Physics::DestroyWithParent, sp, m_Capsule);
}

void Mover::OnUpdate(const double deltaTime, const double tickAlpha) {
	auto& transform = m_Player.GetComponents<Cori::Components::Entity::Transform>();
	auto& renderer = m_Player.GetComponents<Cori::Components::Entity::QuadRenderer>();

	// actual rendering position interpolation between ticks
	if (m_PixelAlignedRender) {
		glm::vec2 pos = Cori::Physics::ToPixels((m_RenderingPosition * tickAlpha + m_OldRenderingPosition * (1.0f - tickAlpha))) + glm::vec2{ 0.0f, renderer.m_HalfSize.y };
		transform.SetLocalPosition({static_cast<int>(pos.x), static_cast<int>(pos.y)});
	}
	else {
		transform.SetLocalPosition(Cori::Physics::ToPixels((m_RenderingPosition * tickAlpha + m_OldRenderingPosition * (1.0f - tickAlpha))) + glm::vec2{ 0.0f, renderer.m_HalfSize.y });
	}

	if (m_Velocity.x < 0.0f) {
		renderer.m_FlipX = true;
	}
	else if (m_Velocity.x > 0.0f) {
		renderer.m_FlipX = false;
	}

	if (m_CanWallJump) {
		if (m_WallJumpDirection == 1) {
			renderer.m_FlipX = true;
		}
		else if (m_WallJumpDirection == -1) {
			renderer.m_FlipX = false;
		}
	}
}

void Mover::OnTickUpdate(const float timeStep, MainCamera& mainCamera) {
	auto& fsm = m_Player.GetComponents < Cori::Components::Entity::StateMachine>();
	
	// vvv double jump raycast checks, to avoid double jumping when player is almoust touching the ground or the wall 
	{
		m_GroundRayStart = { m_Transform.p.x + m_Velocity.x * timeStep + m_Capsule.radius * Cori::Math::Sign(m_Velocity.x), m_Transform.p.y - (m_PogoLengthScale * m_Capsule.radius + m_Capsule.radius - m_Capsule.center1.y) };
		m_GroundRayEnd = { m_GroundRayStart.x, m_GroundRayStart.y + (m_Velocity.y * timeStep * m_JumpBufferTicks / m_DoubleJumpRayModifierForRegular) };
		{
			Cori::Physics::Vec2 rayTranslation = b2Sub(m_GroundRayEnd, m_GroundRayStart);
			Cori::Physics::RayResult rayResult{};

			rayResult.hit = false;
			if (rayTranslation.y < 0.0f) {
				rayResult = m_World.CastRayClosest(m_GroundRayStart, rayTranslation, b2DefaultQueryFilter());
			}

			m_NearGround = rayResult.hit;
		}

		m_WallRayStart = { m_Transform.p.x + m_Capsule.radius * Cori::Math::Sign(m_Velocity.x) + m_Velocity.x * timeStep, m_Transform.p.y + (m_Velocity.y * timeStep * m_WallJumpBufferTicks) };
		m_WallRayEnd = { m_WallRayStart.x + (m_Velocity.x * timeStep * m_WallJumpBufferTicks / m_DoubleJumpRayModifierForWall), m_WallRayStart.y };

		{
			Cori::Physics::Vec2 rayTranslation = b2Sub(m_WallRayEnd, m_WallRayStart);
			Cori::Physics::RayResult rayResult{};

			rayResult = m_World.CastRayClosest(m_WallRayStart, rayTranslation, b2DefaultQueryFilter());
			m_NearWall = rayResult.hit;
		}

		if (m_NearGround) {
			m_NearWall = false;
		}
	}
	// ^^^

	bool bufferFallState = false;
	bool bufferAscendingState = false;

	if (m_OnGround && !m_OldOnGround) {
		float traumaToAdd = 0.15f * (std::sqrt(m_LastFallingDistance) / 4.0f);
		CORI_DEBUG("{} {}", traumaToAdd, m_LastFallingDistance);
		mainCamera.AddTrauma(traumaToAdd);
	}

	// fast fall
	m_Gravity = m_GravityDefault;
	if (m_Velocity.y < -0.35f - m_Gravity * m_FastFallGravityModifier * timeStep) {
		m_Gravity *= m_FastFallGravityModifier;
		bufferFallState = true;
		m_LastFallingDistance += -(m_Velocity.y * timeStep);
	} else {
		m_LastFallingDistance = 0.0f;
	}


	//if (std::abs(m_Velocity.y) > 1.0f && (m_Jumping || m_WallJumping || m_DoubleJumping)) {
	if (std::abs(m_Velocity.y) > 1.0f) {

		bufferAscendingState = true;
	}

	m_CanWallJump = false;

	// wall collision normal response, related to wall slide and wall jump
	int count = m_PlaneCount;
	for (int i = 0; i < count; ++i)
	{
		b2Plane plane = m_Planes[i].plane;

		if (std::abs(std::round(plane.normal.x * 10000.0f)) == 10000) {
			m_CanWallJump = true;
			bufferFallState = false;
			bufferAscendingState = false;
			if (!m_OnGround) {
				fsm.SetStateIfNotInState<States::Player::WallSlide>();
				m_LastFallingDistance = 0.0f;
			}
			m_WallJumpDirection = Cori::Math::Sign(plane.normal.x);
			break;
		}
	}

	if (bufferFallState && !m_CanWallJump) {
		fsm.SetStateIfNotInState<States::Player::Fall>();
	}

	if (bufferAscendingState && !m_CanWallJump) {
		if (fsm.IsInState<States::Player::WallSlide>()) {
			fsm.SetState<States::Player::Ascending>();
		}
	}

	float throttle = 0.0f;

	// vvv left/right movement and wall slide
	{
		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_A)) {
			if (m_CanWallJump && m_WallJumpDirection == 1 && m_Velocity.y < -0.1f - m_Gravity * timeStep) {
				m_Gravity = 0.0f;
				m_Velocity.y = -m_WallSlideSpeed;
			}
			throttle -= 1.0f;
			if (m_OnGround && std::abs(m_Velocity.x) > 0.0f) {
				fsm.SetStateIfNotInState<States::Player::Run>();
			}
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_D)) {
			if (m_CanWallJump && m_WallJumpDirection == -1 && m_Velocity.y < -0.1f - m_Gravity * timeStep) {
				m_Gravity = 0.0f;
				m_Velocity.y = -m_WallSlideSpeed;
			}
			throttle += 1.0f;
			if (m_OnGround && std::abs(m_Velocity.x) > 0.0f) {
				fsm.SetStateIfNotInState<States::Player::Run>();
			}
		}
	}
	// ^^^

	// vvv this whole part is responsible for: jump. double jump, wall jump, jump buffering, wall jump buffering, variable jump, variable double jump, variable wall jump, coyote time
	{
		// vvv coyote time
		if (!m_OnGround && m_OldOnGround) {
			m_JumpCoyoteTimeTickTimer = 0;
		}

		m_OldOnGround = m_OnGround;

		if (m_JumpCoyoteTimeTickTimer < 128) {
			m_JumpCoyoteTimeTickTimer++;
		}
		// ^^^

		if (m_OnGround || m_CanWallJump) {
			m_CanDoubleJump = true;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_SPACE)) {
			if (m_CanDoubleJump && !m_OnGround && !m_CanWallJump && m_JumpButtonReleased && !m_Jumping && !m_NearGround && !m_NearWall && m_JumpCoyoteTimeTickTimer > m_JumpCoyoteTimeTicks) { // double jump initial action
				m_Velocity.y = m_DoubleJumpStartSpeed;
				m_JumpButtonReleased = false;
				m_CanDoubleJump = false;

				CORI_DEBUG("DOUBLE");

				fsm.SetState<States::Player::DoubleJump>();

				m_DoubleJumping = true;
				m_DoubleJumpVariableTickTimer = 0;
			}
			else if (!m_OnGround && m_DoubleJumpVariableTickTimer <= m_DoubleJumpVariableTicks) { // variable double jump action
				m_Velocity.y += m_DoubleJumpVariableSpeed;
				m_DoubleJumpVariableTickTimer++;
			}

			if (m_JumpButtonReleased && (m_JumpBufferTickTimer > m_JumpBufferTicks || m_WallJumpBufferTickTimer > m_WallJumpBufferTicks) && !m_Jumping && !m_DoubleJumping && !m_WallJumping) { 
				CORI_INFO("WALL CAST {}", m_NearWall);
				CORI_INFO("GROUND CAST {}", m_NearGround);

				m_JumpButtonReleased = false;
				m_WallJumpBufferTickTimer = 0;
				m_JumpBufferTickTimer = 0;
			}
			else if (!m_OnGround && m_Jumping && m_JumpVariableTickTimer <= m_JumpVariableTicks) { // variable jump action
				m_Velocity.y += m_JumpVariableSpeed;
				m_JumpVariableTickTimer++;
			}
			else if (!m_OnGround && m_WallJumping && m_WallJumpVariableTickTimer <= m_WallJumpVariableTicks) { // variable wall jump action
				m_Velocity.y += m_WallJumpVariableSpeed;
				m_Velocity.x += m_WallJumpVariableSideSpeed * m_WallJumpDirection;
				m_WallJumpVariableTickTimer++;
			}
		}
		else {
			m_JumpVariableTickTimer = 128;
			m_WallJumpVariableTickTimer = 128;
			m_DoubleJumpVariableTickTimer = 128;
			m_JumpButtonReleased = true;

			m_Jumping = false;
			m_WallJumping = false;
			m_DoubleJumping = false;

			//m_WallJumpDirection = 0;
		}

		if (m_CanWallJump && !m_OnGround && m_WallJumpBufferTickTimer <= m_WallJumpBufferTicks && !m_Jumping) { // wall jump initial action
			m_Velocity.y = m_WallJumpStartSpeed;
			m_Velocity.x = m_WallJumpStartSideSpeed * m_WallJumpDirection;
			m_CanWallJump = false;

			CORI_DEBUG("WALL");

			fsm.SetState<States::Player::WallJump>();

			m_WallJumping = true;
			m_WallJumpVariableTickTimer = 0;
			m_WallJumpBufferTickTimer = 128;
		}
		else if (m_CanWallJump) { // wall jump buffer
			m_WallJumpBufferTickTimer = 128;
		}
		if (m_WallJumpBufferTickTimer < 128) {
			m_WallJumpBufferTickTimer++;
		}


		if ((m_OnGround || m_JumpCoyoteTimeTickTimer <= m_JumpCoyoteTimeTicks) && m_JumpBufferTickTimer <= m_JumpBufferTicks && !m_WallJumping) { // jump initial action
			m_Velocity.y = m_JumpStartSpeed;

			if (m_JumpCoyoteTimeTickTimer <= m_JumpCoyoteTimeTicks) {
				CORI_INFO("COYOTE");
			}
			else if (m_OnGround) {
				CORI_INFO("GROUND");
			}

			m_OnGround = false;

			CORI_DEBUG("REGULAR");

			fsm.SetState<States::Player::Jump>();

			m_Jumping = true;
			m_JumpVariableTickTimer = 0;
			m_JumpBufferTickTimer = 128;
		}
		else if ((m_OnGround || m_JumpCoyoteTimeTickTimer > m_JumpCoyoteTimeTicks) && m_JumpBufferTickTimer > m_JumpBufferTicks) { // jump buffer and coyote time reset
			m_JumpBufferTickTimer = 128;
			m_JumpCoyoteTimeTickTimer = 128;
		}
		if (m_JumpBufferTickTimer < 128) {
			m_JumpBufferTickTimer++;
		}
	}
	// ^^^

	// set idle if still
	if (m_OnGround && std::abs(m_Velocity.x) < m_MinSpeedForRunState) {
		fsm.SetStateIfNotInState<States::Player::Idle>();
	}

	m_OldTransform = m_Transform;
	
	SolveMove(timeStep, throttle);

	// update kinematic body position meant for sensor use
	auto& rb = m_Player.GetComponents<Cori::Components::Entity::Rigidbody>();
	constexpr float tolerance = 0.01f;
	b2Vec2 delta = m_OldTransform.p - m_Transform.p;
	if (std::abs(delta.x) > tolerance * tolerance || std::abs(delta.y) > tolerance * tolerance) {
		rb.SetTargetTransform({ {m_Transform.p.x + m_Velocity.x * timeStep, m_Transform.p.y + m_Velocity.y * timeStep}, b2Rot_identity }, timeStep);
	}
	else {
		rb.SetLinearVelocity({ 0.0f, 0.0f });
	}

	// for debug draw
	m_P1 = b2TransformPoint(m_Transform, m_Capsule.center1);
	m_P2 = b2TransformPoint(m_Transform, m_Capsule.center2);

	// rendering positions update (for interpolation)
	m_OldRenderingPosition = m_RenderingPosition;
	if (m_Velocity.y < -0.1f - m_Gravity * timeStep) {
		if (m_CastResult.hit == false) {
			m_RenderingPosition = m_Origin + m_Translation + m_Velocity * (timeStep * 1.5f);
		}
		else {
			m_RenderingPosition = m_Origin + m_CastResult.fraction * m_Translation + m_Velocity * (timeStep * 1.5f);
		}
	}
	else {
		if (m_CastResult.hit == false) {
			m_RenderingPosition = m_Origin + m_Translation + Cori::Physics::Vec2{ m_Velocity.x * (timeStep * 1.5f), 0.0f };
		}
		else {
			m_RenderingPosition = m_Origin + m_CastResult.fraction * m_Translation + Cori::Physics::Vec2{ m_Velocity.x * (timeStep * 1.5f), 0.0f };
		}
	}
}

bool Mover::PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult* planeResult, void* context) {
	assert(planeResult->hit == true);

	Mover* self = static_cast<Mover*>(context);

	if (self->m_PlaneCount < m_PlaneCapacity) {
		assert(b2IsValidPlane(planeResult->plane));
		self->m_Planes[self->m_PlaneCount] = { planeResult->plane, FLT_MAX, 0.0f, true };
		self->m_PlaneCount += 1;
	}

	return true;
}

void Mover::UpdateGui() {
	ImGui::Begin("Mover");

	ImGui::SliderFloat("Jump Start Speed", &m_JumpStartSpeed, 0.0f, 40.0f, "%.1f");
	ImGui::SliderFloat("Jump Variable Speed", &m_JumpVariableSpeed, 0.0f, 10.0f, "%.1f");
	ImGui::SliderInt("Jump Variable Ticks", &m_JumpVariableTicks, 0, 15);
	ImGui::SliderInt("Jump Buffer Ticks", &m_JumpBufferTicks, 0, 30);
	ImGui::SliderInt("Jump Coyote Time Ticks", &m_JumpCoyoteTimeTicks, 0, 30);

	ImGui::Separator();

	ImGui::SliderFloat("Wall Jump Start Speed", &m_WallJumpStartSpeed, 0.0f, 40.0f, "%.1f");
	ImGui::SliderFloat("Wall Jump Variable Speed", &m_WallJumpVariableSpeed, 0.0f, 10.0f, "%.1f");
	ImGui::SliderFloat("Wall Jump Start Side Speed", &m_WallJumpStartSideSpeed, 0.0f, 40.0f, "%.1f");
	ImGui::SliderFloat("Wall Jump Variable Side Speed", &m_WallJumpVariableSideSpeed, 0.0f, 10.0f, "%.1f");
	ImGui::SliderInt("Wall Jump Variable Ticks", &m_WallJumpVariableTicks, 0, 15);
	ImGui::SliderInt("Wall Jump Buffer Ticks", &m_WallJumpBufferTicks, 0, 30);

	ImGui::Separator();

	ImGui::SliderFloat("Double Jump Start Speed", &m_DoubleJumpStartSpeed, 0.0f, 40.0f, "%.1f");
	ImGui::SliderFloat("Double Jump Variable Speed", &m_DoubleJumpVariableSpeed, 0.0f, 10.0f, "%.1f");
	ImGui::SliderInt("Double Jump Variable Ticks", &m_DoubleJumpVariableTicks, 0, 15);
	ImGui::SliderFloat("Double Jump Ray Modifier For Regular Jump", &m_DoubleJumpRayModifierForRegular, 1.0f, 8.0f, "%.1f");
	ImGui::SliderFloat("Double Jump Ray Modifier For Wall Jump", &m_DoubleJumpRayModifierForWall, 1.0f, 8.0f, "%.1f");

	ImGui::Separator();

	ImGui::SliderFloat("Min Speed", &m_MinSpeed, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat("Max Speed", &m_MaxSpeed, 0.0f, 20.0f, "%.0f");
	ImGui::SliderFloat("Stop Speed", &m_StopSpeed, 0.0f, 40.0f, "%.1f");

	ImGui::Separator();

	ImGui::SliderFloat("Gravity Default", &m_GravityDefault, 0.0f, 100.0f, "%.1f");
	ImGui::SliderFloat("Fast Fall Gravity Modifier", &m_FastFallGravityModifier, 0.0f, 3.0f, "%.2f");

	ImGui::Separator();

	ImGui::SliderFloat("Acceleration", &m_Acceleration, 0.0f, 100.0f, "%.0f");
	ImGui::SliderFloat("Friction", &m_Friction, 0.0f, 40.0f, "%.1f");
	ImGui::SliderFloat("Air Steer", &m_AirSteer, 0.0f, 1.0f, "%.2f");

	ImGui::Separator();

	ImGui::SliderFloat("Pogo Hertz", &m_PogoHertz, 0.0f, 30.0f, "%.0f");
	ImGui::SliderFloat("Pogo Damping", &m_PogoDampingRatio, 0.0f, 4.0f, "%.1f");
	ImGui::SliderFloat("Pogo Length Scale", &m_PogoLengthScale, 0.1f, 10.0f, "%.2f");
	ImGui::SliderFloat("Segment Offset", &m_SegmentOffset, 0.1f, 4.0f, "%.2f");

	ImGui::Separator();

	ImGui::SliderFloat("Wall Slide Speed", &m_WallSlideSpeed, 0.1f, 10.0f, "%.2f");

	ImGui::Separator();

	ImGui::SliderFloat("Minimal Speed For Run State", &m_MinSpeedForRunState, 0.1f, 10.0f, "%.2f");
	ImGui::Checkbox("Draw Player Pixel Aligned to Camera Canvas", &m_PixelAlignedRender);

	//ImGui::SliderFloat("Test Value1", &test1, 0.1f, 20.0f, "%.2f");

	ImGui::Separator();

	if (ImGui::Button("Save Settings")) {
		SaveSettings("../../config/mover.json");
	}

	if (ImGui::Button("Load Settings")) {
		LoadSettings("../../config/mover.json");
	}

	ImGui::End();
}

void Mover::DebugDraw(float test) {
	if (m_CastResult.hit == false) {
		b2Vec2 delta = m_Translation + m_Velocity * (1.0f / 60.0f);
		Cori::Layer::debug_renderer.DrawLine(m_Origin + m_Velocity * (1.0f / 60.0f), m_Origin + delta, b2_colorPurple);
		Cori::Layer::debug_renderer.DrawLine(m_Segment.point1 + delta, m_Segment.point2 + delta, b2_colorPurple);
	}
	else {
		b2Vec2 delta = m_CastResult.fraction * m_Translation + m_Velocity * (1.0f / 60.0f);
		Cori::Layer::debug_renderer.DrawLine(m_Origin + m_Velocity * (1.0f / 60.0f), m_Origin + delta, b2_colorPurple);
		Cori::Layer::debug_renderer.DrawLine(m_Segment.point1 + delta, m_Segment.point2 + delta, b2_colorPlum);
	}

	int count = m_PlaneCount;
	for (int i = 0; i < count; ++i)
	{
		b2Plane plane = m_Planes[i].plane;
		b2Vec2 p1 = m_Transform.p + (plane.offset - m_Capsule.radius) * plane.normal;
		b2Vec2 p2 = p1 + 0.1f * plane.normal;
		Cori::Layer::debug_renderer.DrawPoint(p1, 5.0f, b2_colorYellow);
		Cori::Layer::debug_renderer.DrawLine(p1, p2, b2_colorYellow);
	}

	b2HexColor color = m_OnGround ? b2_colorOrange : b2_colorAquamarine;
	Cori::Layer::debug_renderer.DrawCapsuleFilled(m_P1, m_P2, m_Capsule.radius, color);
	Cori::Layer::debug_renderer.DrawLine(m_Transform.p, m_Transform.p + m_Velocity, b2_colorPurple);

	Cori::Layer::debug_renderer.DrawPoint(m_P1, 8.0f, b2_colorPink);

	Cori::Layer::debug_renderer.DrawLine(m_GroundRayStart, m_GroundRayEnd, b2_colorWhite);

	Cori::Layer::debug_renderer.DrawLine(m_WallRayStart, m_WallRayEnd, b2_colorWhite);

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 21.9f }, fmt::color::white, "Velocity: " + Cori::Physics::Vec2ToString(m_Velocity));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 21.6f }, fmt::color::white, "Velocity * ts: " + Cori::Physics::Vec2ToString({ m_Velocity.x * (1.0f / 60.0f), m_Velocity.y * (1.0f / 60.0f) }));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 21.3f }, fmt::color::white, "Position: " + Cori::Physics::Vec2ToString(m_Transform.p));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 20.7f }, fmt::color::white, "m_JumpVariableTickTimer: " + std::to_string(m_JumpVariableTickTimer));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 20.4f }, fmt::color::white, "m_JumpBufferTickTimer: " + std::to_string(m_JumpBufferTickTimer));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 20.1f }, fmt::color::white, "m_JumpCoyoteTimeTickTimer: " + std::to_string(m_JumpCoyoteTimeTickTimer));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 19.5f }, fmt::color::white, "m_WallJumpBufferTickTimer: " + std::to_string(m_WallJumpBufferTickTimer));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 19.2f }, fmt::color::white, "m_WallJumpVariableTickTimer: " + std::to_string(m_WallJumpVariableTickTimer));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 18.6f }, fmt::color::white, "m_DoubleJumpVariableTickTimer: " + std::to_string(m_DoubleJumpVariableTickTimer));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 18.0f }, fmt::color::white, "m_NearGround: " + Cori::Logger::BoolAlpha(m_NearGround));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 17.7f }, fmt::color::white, "m_NearWall: " + Cori::Logger::BoolAlpha(m_NearWall));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 17.1f }, fmt::color::white, "m_Gravity: " + std::to_string(m_Gravity));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 16.5f }, fmt::color::white, "m_Jumping: " + Cori::Logger::BoolAlpha(m_Jumping));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 16.2f }, fmt::color::white, "m_WallJumping: " + Cori::Logger::BoolAlpha(m_WallJumping));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 15.9f }, fmt::color::white, "m_DoubleJumping: " + Cori::Logger::BoolAlpha(m_DoubleJumping));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 15.6f }, fmt::color::white, "m_CanWallJump: " + Cori::Logger::BoolAlpha(m_CanWallJump));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 15.3f }, fmt::color::white, "m_CanDoubleJump: " + Cori::Logger::BoolAlpha(m_CanDoubleJump));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 15.0f }, fmt::color::white, "m_OnGround: " + Cori::Logger::BoolAlpha(m_OnGround));
	Cori::Layer::debug_renderer.DrawText({ 1.1f, 14.7f }, fmt::color::white, "m_WallJumpDirection: " + std::to_string(m_WallJumpDirection));

	Cori::Layer::debug_renderer.DrawText({ 1.1f, 14.1f }, fmt::color::white, "Current Player State: " + std::string(m_Player.GetComponents<Cori::Components::Entity::StateMachine>().GetCurrentState()->GetDebugName()));
}

void Mover::SolveMove(const float timeStep, float throttle) {
	// Friction
	float speed = b2Length(m_Velocity);
	if (speed < m_MinSpeed)
	{
		m_Velocity.x = 0.0f;
		m_Velocity.y = 0.0f;
	}
	else if (m_OnGround)
	{
		// Linear damping above stopSpeed and fixed reduction below stopSpeed
		float control = speed < m_StopSpeed ? m_StopSpeed : speed;

		// friction has units of 1/time
		float drop = control * m_Friction * timeStep;
		float newSpeed = b2MaxFloat(0.0f, speed - drop);
		m_Velocity *= newSpeed / speed;
	}

	b2Vec2 desiredVelocity = { m_MaxSpeed * throttle, 0.0f };
	float desiredSpeed;
	b2Vec2 desiredDirection = b2GetLengthAndNormalize(&desiredSpeed, desiredVelocity);

	if (desiredSpeed > m_MaxSpeed)
	{
		desiredSpeed = m_MaxSpeed;
	}

	if (m_OnGround)
	{
		m_Velocity.y = 0.0f;
	}

	// Accelerate
	float currentSpeed = b2Dot(m_Velocity, desiredDirection);
	float addSpeed = desiredSpeed - currentSpeed;
	if (addSpeed > 0.0f) {
		float steer = m_OnGround ? 1.0f : m_AirSteer;
		float accelSpeed = steer * m_Acceleration * m_MaxSpeed * timeStep;
		if (accelSpeed > addSpeed)
		{
			accelSpeed = addSpeed;
		}

		m_Velocity += accelSpeed * desiredDirection;
	}

	m_Velocity.y -= m_Gravity * timeStep;

	float pogoRestLength = m_PogoLengthScale * m_Capsule.radius;
	float rayLength = pogoRestLength + m_Capsule.radius;
	m_Origin = b2TransformPoint(m_Transform, m_Capsule.center1);
	b2Vec2 segmentOffset = { m_SegmentOffset * m_Capsule.radius, 0.0f };
	m_Segment = Cori::Physics::Segment::Create(m_Origin - segmentOffset, m_Origin + segmentOffset);

	b2ShapeProxy proxy = {};
	b2QueryFilter pogoFilter = { Cori::Physics::CollisionBits::MoverBit, Cori::Physics::CollisionBits::StaticBit | Cori::Physics::CollisionBits::DynamicBit };

	proxy = b2MakeProxy(&m_Segment.point1, 2, 0.0f);
	m_Translation = { 0.0f, -rayLength };

	m_CastResult = {};
	m_World.Cast(proxy, m_Translation, pogoFilter, [&](Cori::Physics::ShapeRef shape, b2Vec2 point, b2Vec2 normal, float fraction) {
		m_CastResult.point = point;
		m_CastResult.normal = normal;
		m_CastResult.shape = shape;
		m_CastResult.fraction = fraction;
		m_CastResult.hit = true;
		return fraction;
		});

	// Avoid snapping to ground if still going up
	if (m_OnGround == false)
	{
		m_OnGround = m_CastResult.hit && m_Velocity.y <= 0.01f;
	}
	else
	{
		m_OnGround = m_CastResult.hit;
	}

	if (m_CastResult.hit == false)
	{
		m_PogoVelocity = 0.0f;
	}
	else
	{
		float pogoCurrentLength = m_CastResult.fraction * rayLength;

		float offset = pogoCurrentLength - pogoRestLength;

		m_PogoVelocity = b2SpringDamper(m_PogoHertz, m_PogoDampingRatio, offset, m_PogoVelocity, timeStep);

		m_CastResult.shape.GetBody().ApplyForce({ 0.0f, -50.0f }, m_CastResult.point, true);
	}

	b2Vec2 target = m_Transform.p + timeStep * m_Velocity + timeStep * m_PogoVelocity * b2Vec2{ 0.0f, 1.0f };

	// Mover overlap filter
	b2QueryFilter collideFilter = { Cori::Physics::MoverBit, Cori::Physics::CollisionBits::StaticBit | Cori::Physics::CollisionBits::DynamicBit | Cori::Physics::CollisionBits::MoverBit };

	// Movers don't sweep against other movers, allows for soft collision
	b2QueryFilter castFilter = { Cori::Physics::CollisionBits::MoverBit, Cori::Physics::CollisionBits::StaticBit | Cori::Physics::CollisionBits::DynamicBit };

	m_TotalIterations = 0;
	constexpr float tolerance = 0.01f;

	for (int iteration = 0; iteration < 5; ++iteration)
	{
		m_PlaneCount = 0;

		b2Capsule mover;
		mover.center1 = b2TransformPoint(m_Transform, m_Capsule.center1);

		mover.center2 = b2TransformPoint(m_Transform, m_Capsule.center2);
		mover.radius = m_Capsule.radius;

		m_World.CollideMover(mover, collideFilter, PlaneResultFcn, this);


		b2PlaneSolverResult result = b2SolvePlanes(target - m_Transform.p, m_Planes, m_PlaneCount);

		m_TotalIterations += result.iterationCount;

		float fraction = m_World.CastMover(mover, result.translation, castFilter);

		b2Vec2 delta = fraction * result.translation;
		m_Transform.p += delta;

		if (b2LengthSquared(delta) < tolerance * tolerance)
		{
			break;
		}
	}

	m_Velocity = b2ClipVector(m_Velocity, m_Planes, m_PlaneCount);
}

void Mover::SaveSettings(const std::filesystem::path& filepath) {
	Params currentParams;

	currentParams.jumpStartSpeed = m_JumpStartSpeed;
	currentParams.jumpVariableSpeed = m_JumpVariableSpeed;
	currentParams.jumpVariableTicks = m_JumpVariableTicks;
	currentParams.jumpBufferTicks = m_JumpBufferTicks;
	currentParams.jumpCoyoteTimeTicks = m_JumpCoyoteTimeTicks;
	currentParams.wallJumpStartSpeed = m_WallJumpStartSpeed;
	currentParams.wallJumpVariableSpeed = m_WallJumpVariableSpeed;
	currentParams.wallJumpStartSideSpeed = m_WallJumpStartSideSpeed;
	currentParams.wallJumpVariableSideSpeed = m_WallJumpVariableSideSpeed;
	currentParams.wallJumpVariableTicks = m_WallJumpVariableTicks;
	currentParams.wallJumpBufferTicks = m_WallJumpBufferTicks;
	currentParams.doubleJumpStartSpeed = m_DoubleJumpStartSpeed;
	currentParams.doubleJumpVariableSpeed = m_DoubleJumpVariableSpeed;
	currentParams.doubleJumpVariableTicks = m_DoubleJumpVariableTicks;
	currentParams.doubleJumpRayModifierForRegular = m_DoubleJumpRayModifierForRegular;
	currentParams.doubleJumpRayModifierForWall = m_DoubleJumpRayModifierForWall;
	currentParams.maxSpeed = m_MaxSpeed;
	currentParams.minSpeed = m_MinSpeed;
	currentParams.stopSpeed = m_StopSpeed;
	currentParams.acceleration = m_Acceleration;
	currentParams.airSteer = m_AirSteer;
	currentParams.friction = m_Friction;
	currentParams.gravityDefault = m_GravityDefault;
	currentParams.fastFallGravityModifier = m_FastFallGravityModifier;
	currentParams.pogoHertz = m_PogoHertz;
	currentParams.pogoDampingRatio = m_PogoDampingRatio;
	currentParams.pogoLengthScale = m_PogoLengthScale;
	currentParams.segmentOffset = m_SegmentOffset;
	currentParams.wallSlideSpeed = m_WallSlideSpeed;
	currentParams.minSpeedForRunState = m_MinSpeedForRunState;

	if (auto result = Cori::JsonSerializer::Save(currentParams, filepath); !result) {
		CORI_INFO_TAGGED({ "Mover" }, "Failed to save mover settings to: {} : {}", filepath.string(), result.error());
	}
	else {
		CORI_INFO_TAGGED({ "Mover" }, "Mover settings saved successfully to: {} ", filepath.string());
	}
}

void Mover::LoadSettings(const std::filesystem::path& filepath) {
	auto loadedParamsResult = Cori::JsonSerializer::Load<Params>(filepath);

	if (!loadedParamsResult) {
		CORI_INFO_TAGGED({ "Mover" }, "Failed to load mover settings from: {} : {}", filepath.string(), loadedParamsResult.error());
		return;
	}

	const Params& loadedParams = *loadedParamsResult;

	m_JumpStartSpeed = loadedParams.jumpStartSpeed;
	m_JumpVariableSpeed = loadedParams.jumpVariableSpeed;
	m_JumpVariableTicks = loadedParams.jumpVariableTicks;
	m_JumpBufferTicks = loadedParams.jumpBufferTicks;
	m_JumpCoyoteTimeTicks = loadedParams.jumpCoyoteTimeTicks;
	m_WallJumpStartSpeed = loadedParams.wallJumpStartSpeed;
	m_WallJumpVariableSpeed = loadedParams.wallJumpVariableSpeed;
	m_WallJumpStartSideSpeed = loadedParams.wallJumpStartSideSpeed;
	m_WallJumpVariableSideSpeed = loadedParams.wallJumpVariableSideSpeed;
	m_WallJumpVariableTicks = loadedParams.wallJumpVariableTicks;
	m_WallJumpBufferTicks = loadedParams.wallJumpBufferTicks;
	m_DoubleJumpStartSpeed = loadedParams.doubleJumpStartSpeed;
	m_DoubleJumpVariableSpeed = loadedParams.doubleJumpVariableSpeed;
	m_DoubleJumpVariableTicks = loadedParams.doubleJumpVariableTicks;
	m_DoubleJumpRayModifierForRegular = loadedParams.doubleJumpRayModifierForRegular;
	m_DoubleJumpRayModifierForWall = loadedParams.doubleJumpRayModifierForWall;
	m_MaxSpeed = loadedParams.maxSpeed;
	m_MinSpeed = loadedParams.minSpeed;
	m_StopSpeed = loadedParams.stopSpeed;
	m_Acceleration = loadedParams.acceleration;
	m_AirSteer = loadedParams.airSteer;
	m_Friction = loadedParams.friction;
	m_GravityDefault = loadedParams.gravityDefault;
	m_FastFallGravityModifier = loadedParams.fastFallGravityModifier;
	m_PogoHertz = loadedParams.pogoHertz;
	m_PogoDampingRatio = loadedParams.pogoDampingRatio;
	m_PogoLengthScale = loadedParams.pogoLengthScale;
	m_SegmentOffset = loadedParams.segmentOffset;
	m_WallSlideSpeed = loadedParams.wallSlideSpeed;
	m_MinSpeedForRunState = loadedParams.minSpeedForRunState;


	CORI_INFO_TAGGED({ "Mover" }, "Mover settings loaded successfully from: {} ", filepath.string());
}
