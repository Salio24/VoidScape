#pragma once
#include <Cori.hpp>

class Mover {
public:


	struct Params {
		Params() = default;

		Cori::Physics::Vec2 position{ 0.0f, 0.0f };
		float jumpStartSpeed{ 10.0f };
		float jumpVariableSpeed{ 1.0f };
		int   jumpVariableTicks{ 10 };
		int   jumpBufferTicks{ 8 };
		int   jumpCoyoteTimeTicks{ 4 };

		float wallJumpStartSpeed{ 10.0f };
		float wallJumpVariableSpeed{ 1.0f };
		float wallJumpStartSideSpeed{ 10.0f };
		float wallJumpVariableSideSpeed{ 0.0f };
		int   wallJumpVariableTicks{ 10 };
		int   wallJumpBufferTicks{ 4 };

		float doubleJumpStartSpeed{ 10.0f };
		float doubleJumpVariableSpeed{ 1.0f };
		int   doubleJumpVariableTicks{ 10 };
		float doubleJumpRayModifierForRegular{ 2.0f };
		float doubleJumpRayModifierForWall{ 2.0f };

		float maxSpeed{ 6.0f };
		float minSpeed{ 0.1f };
		float stopSpeed{ 3.0f };
		float acceleration{ 20.0f };
		float airSteer{ 0.2f };
		float friction{ 8.0f };
		float gravityDefault{ 30.0f };
		float fastFallGravityModifier{ 1.5f };
		float pogoHertz{ 5.0f };
		float pogoDampingRatio{ 0.8f };
		float pogoLengthScale{ 2.0f };
		float segmentOffset{ 0.9f };
		float wallSlideSpeed{ 1.5f };
	};

	Mover(const Cori::Physics::Capsule& capsule, Cori::Physics::WorldRef world, const Params& def) : m_Capsule(capsule), m_World(world), m_Transform{ def.position, b2Rot_identity }, 
			m_JumpStartSpeed(def.jumpStartSpeed), m_JumpVariableSpeed(def.jumpVariableSpeed), m_JumpVariableTicks(def.jumpVariableTicks), m_JumpBufferTicks(def.jumpBufferTicks), m_JumpCoyoteTimeTicks(def.jumpCoyoteTimeTicks),
			m_WallJumpStartSpeed(def.wallJumpStartSpeed), m_WallJumpVariableSpeed(def.wallJumpVariableSpeed), m_WallJumpStartSideSpeed(def.wallJumpStartSideSpeed), m_WallJumpVariableSideSpeed(def.wallJumpVariableSideSpeed), m_WallJumpVariableTicks(def.wallJumpVariableTicks), m_WallJumpBufferTicks(def.wallJumpBufferTicks),
			m_DoubleJumpStartSpeed(def.doubleJumpStartSpeed), m_DoubleJumpVariableSpeed(def.doubleJumpVariableSpeed), m_DoubleJumpVariableTicks(def.doubleJumpVariableTicks), m_DoubleJumpRayModifierForRegular(def.doubleJumpRayModifierForRegular), m_DoubleJumpRayModifierForWall(def.doubleJumpRayModifierForWall),
			m_MaxSpeed(def.maxSpeed), m_MinSpeed(def.minSpeed), m_StopSpeed(def.stopSpeed), m_Acceleration(def.acceleration), m_AirSteer(def.airSteer),
			m_Friction(def.friction), m_GravityDefault(def.gravityDefault), m_FastFallGravityModifier(def.fastFallGravityModifier), m_PogoHertz(def.pogoHertz), m_PogoDampingRatio(def.pogoDampingRatio), m_PogoLengthScale(def.pogoLengthScale), m_SegmentOffset(def.segmentOffset), m_WallSlideSpeed(def.wallSlideSpeed) {
		Cori::Physics::Body::Params bp;
		bp.type = b2_kinematicBody;
		bp.position = m_Transform.p;
		bp.fixedRotation = true;
		bp.rotation = b2Rot_identity;

		m_SensorVisitorBody = m_World.CreateBody(Cori::Physics::DestroyWithParent, bp);

		Cori::Physics::Shape::Params sp;
		sp.filter.maskBits = Cori::Physics::CollisionBits::SensorBit;
		sp.isSensor = true;
		sp.enableSensorEvents = true;
		sp.userData = &m_Player;

		m_SensorVisitorBody.CreateShape(Cori::Physics::DestroyWithParent, sp, m_Capsule);
	}

	~Mover() = default;

	void BindPlayer(const Cori::Entity& player) {
		m_Player = player;
	}

	void UnbindPlayer() {
		m_Player = Cori::Entity{};
	}


	void SolveMove(const float timeStep, float throttle) {
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

			Cori::Physics::Vec2 t = { m_Capsule.center2.x, m_Capsule.center2.y * test };

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

	void UpdateGui() {

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


		ImGui::End();
	}

	void DebugDraw() {

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
		Cori::Layer::debug_renderer.DrawText({ 1.1f, 21.6f }, fmt::color::white, "Velocity * ts: " + Cori::Physics::Vec2ToString({m_Velocity.x * (1.0f / 60.0f), m_Velocity.y * (1.0f / 60.0f) }));
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

	}

	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult* planeResult, void* context) {
		assert(planeResult->hit == true);

		Mover* self = static_cast<Mover*>(context);

		if (self->m_PlaneCount < m_PlaneCapacity) {
			assert(b2IsValidPlane(planeResult->plane));
			self->m_Planes[self->m_PlaneCount] = { planeResult->plane, FLT_MAX, 0.0f, true };
			self->m_PlaneCount += 1;
		}

		//CORI_DEBUG("Normals: {}", Cori::Physics::Vec2ToString(planeResult->plane.normal));

		return true;
	}

	void OnUpdate(const double deltaTime, const double tickAlpha) {

	}

	void OnTickUpdate(const float timeStep) {
		originToBottom = m_PogoLengthScale * m_Capsule.radius + m_Capsule.radius - m_Capsule.center1.y;

		m_GroundRayStart = { m_Transform.p.x + m_Velocity.x * timeStep + m_Capsule.radius * Cori::Math::Sign(m_Velocity.x), m_Transform.p.y - (m_PogoLengthScale * m_Capsule.radius + m_Capsule.radius - m_Capsule.center1.y) };
		m_GroundRayEnd = { m_GroundRayStart.x, m_GroundRayStart.y + (m_Velocity.y * timeStep * m_JumpBufferTicks / m_DoubleJumpRayModifierForRegular) };
		{
			Cori::Physics::Vec2 rayTranslation = b2Sub(m_GroundRayEnd, m_GroundRayStart);
			b2RayResult rayresult{};

			rayresult.hit = false;
			if (rayTranslation.y < 0.0f) {
				rayresult = m_World.CastRayClosest(m_GroundRayStart, rayTranslation, b2DefaultQueryFilter());
			}

			m_NearGround = rayresult.hit;
		}

		m_WallRayStart = { m_Transform.p.x + m_Capsule.radius * Cori::Math::Sign(m_Velocity.x) + m_Velocity.x * timeStep, m_Transform.p.y + (m_Velocity.y * timeStep * m_WallJumpBufferTicks) };
		m_WallRayEnd = { m_WallRayStart.x + (m_Velocity.x * timeStep * m_WallJumpBufferTicks / m_DoubleJumpRayModifierForWall), m_WallRayStart.y };

		{
			Cori::Physics::Vec2 rayTranslation = b2Sub(m_WallRayEnd, m_WallRayStart);
			b2RayResult rayresult{};

			rayresult = m_World.CastRayClosest(m_WallRayStart, rayTranslation, b2DefaultQueryFilter());
			m_NearWall = rayresult.hit;
		}

		if (m_NearGround) {
			m_NearWall = false;
		}

		//CORI_INFO("cast: {}", m_NearWall);
		
		//CORI_INFO("pr: {}", rayresult.hit);
		
		m_Gravity = m_GravityDefault;
		if (m_Velocity.y < -0.1f - m_Gravity * m_FastFallGravityModifier * timeStep) {
			m_Gravity *= m_FastFallGravityModifier;
		}

		m_CanWallJump = false;

		int count = m_PlaneCount;
		for (int i = 0; i < count; ++i)
		{
			b2Plane plane = m_Planes[i].plane;

			if (std::abs(std::round(plane.normal.x * 10000.0f)) == 10000) {
				m_CanWallJump = true;
				m_WallJumpDirection = Cori::Math::Sign(plane.normal.x);
				//CORI_DEBUG("Normals: {}, bool {}", Cori::Physics::Vec2ToString(plane.normal), m_CanWallJump);
				break;
			}
		}

		float throttle = 0.0f;

		//CORI_DEBUG("Vel: {}", Cori::Physics::Vec2ToString(m_Velocity));

		//CORI_DEBUG("Vel: {}", Cori::Physics::Vec2ToString({m_Velocity.x * timeStep, m_Velocity.y * timeStep}));

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_A)) {
			if (m_CanWallJump && m_WallJumpDirection == 1 && m_Velocity.y < -0.1f - m_Gravity * timeStep) {
				m_Gravity = 0.0f;
				m_Velocity.y = -m_WallSlideSpeed;
			}
			throttle -= 1.0f;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_D)) {
			if (m_CanWallJump && m_WallJumpDirection == -1 && m_Velocity.y < -0.1f - m_Gravity * timeStep) {
				m_Gravity = 0.0f;
				m_Velocity.y = -m_WallSlideSpeed;
			}
			throttle += 1.0f;
		}

		if (!m_OnGround && m_OldOnGround) {
			m_JumpCoyoteTimeTickTimer = 0;
		}
		m_OldOnGround = m_OnGround;


		if (m_JumpCoyoteTimeTickTimer < 128) {
			m_JumpCoyoteTimeTickTimer++;
		}

		if (m_OnGround || m_CanWallJump) {
			m_CanDoubleJump = true;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_SPACE)) {
			if (m_CanDoubleJump && !m_OnGround && !m_CanWallJump && m_JumpButtonReleased && !m_Jumping && !m_NearGround && !m_NearWall && m_JumpCoyoteTimeTickTimer > m_JumpCoyoteTimeTicks) {
				m_Velocity.y = m_DoubleJumpStartSpeed;
				m_JumpButtonReleased = false;
				m_CanDoubleJump = false;
				
				CORI_DEBUG("DOUBLE");


				m_DoubleJumping = true;
				m_DoubleJumpVariableTickTimer = 0;
			}
			else if (!m_OnGround && m_DoubleJumpVariableTickTimer <= m_DoubleJumpVariableTicks) {
				m_Velocity.y += m_DoubleJumpVariableSpeed;
				m_DoubleJumpVariableTickTimer++;
			}
			//if (m_CanWallJump && !m_OnGround && m_JumpButtonReleased && !m_Jumping && !m_DoubleJumping) {
			//	m_Velocity.y = m_WallJumpStartSpeed;
			//	m_Velocity.x = m_WallJumpStartSideSpeed * m_WallJumpDirection;
			//	m_JumpButtonReleased = false;
			//	m_CanWallJump = false;
			//
			//	m_WallJumping = true;
			//	m_WallJumpVariableTickTimer = 0;
			//} 
			//else if (!m_CanWallJump && m_WallJumpVariableTickTimer <= m_WallJumpVariableTicks) {
			//	m_Velocity.y += m_WallJumpVariableSpeed;
			//	m_Velocity.x += m_WallJumpVariableSideSpeed * m_WallJumpDirection;
			//	m_WallJumpVariableTickTimer++;
			//}

			if (m_JumpButtonReleased && (m_JumpBufferTickTimer > m_JumpBufferTicks || m_WallJumpBufferTickTimer > m_WallJumpBufferTicks) && !m_Jumping && !m_DoubleJumping && !m_WallJumping) {
				CORI_INFO("WALL CAST {}", m_NearWall);
				CORI_INFO("GROUND CAST {}", m_NearGround);

				m_JumpButtonReleased = false;
				m_WallJumpBufferTickTimer = 0;
				m_JumpBufferTickTimer = 0;
			}
			else if (!m_OnGround && m_Jumping && m_JumpVariableTickTimer <= m_JumpVariableTicks) {
				m_Velocity.y += m_JumpVariableSpeed;
				m_JumpVariableTickTimer++;
			} 
			else if (!m_OnGround && m_WallJumping && m_WallJumpVariableTickTimer <= m_WallJumpVariableTicks) {
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

			m_WallJumpDirection = 0;
		
		}

		if (m_CanWallJump && !m_OnGround && m_WallJumpBufferTickTimer <= m_WallJumpBufferTicks && !m_Jumping) {
			m_Velocity.y = m_WallJumpStartSpeed;
			m_Velocity.x = m_WallJumpStartSideSpeed * m_WallJumpDirection;
			m_CanWallJump = false;


			CORI_DEBUG("WALL");


			m_WallJumping = true;
			m_WallJumpVariableTickTimer = 0;
			m_WallJumpBufferTickTimer = 128;
		}
		else if (m_CanWallJump) {
			m_WallJumpBufferTickTimer = 128;
		}

		if (m_WallJumpBufferTickTimer < 128) {
			m_WallJumpBufferTickTimer++;
		}

		if (m_WallJumpBufferTickTimer < 128) {
			m_WallJumpBufferTickTimer++;
		}

		if ((m_OnGround || m_JumpCoyoteTimeTickTimer <= m_JumpCoyoteTimeTicks) && m_JumpBufferTickTimer <= m_JumpBufferTicks && !m_WallJumping) {
			m_Velocity.y = m_JumpStartSpeed;
			m_OnGround = false;

			if (m_JumpCoyoteTimeTickTimer <= m_JumpCoyoteTimeTicks) {
				CORI_INFO("COYOTE");
			}
			else if (m_OnGround) {
				CORI_INFO("GROUND");
			}

			CORI_DEBUG("REGULAR");
			// add fast fall

			m_Jumping = true;
			m_JumpVariableTickTimer = 0;
			m_JumpBufferTickTimer = 128;
		} 
		else if (m_OnGround || m_JumpCoyoteTimeTickTimer > m_JumpCoyoteTimeTicks) {
			m_JumpBufferTickTimer = 128;
		}

		if (m_JumpBufferTickTimer < 128) {
			m_JumpBufferTickTimer++;
		}

		//CORI_INFO("ticks: {}", m_JumpCoyoteTimeTickTimer);


		constexpr float tolerance = 0.01f;

		m_OldTransform = m_Transform;

		SolveMove(timeStep, throttle);

		b2Vec2 delta = m_OldTransform.p - m_Transform.p;


		// try to use velocity here
		if (std::abs(delta.x) > tolerance * tolerance || std::abs(delta.y) > tolerance * tolerance) {
			m_SensorVisitorBody.SetTargetTransform({ {m_Transform.p.x + m_Velocity.x * timeStep, m_Transform.p.y + m_Velocity.y * timeStep}, b2Rot_identity }, timeStep);
		}
		else {
			m_SensorVisitorBody.SetLinearVelocity({ 0.0f, 0.0f });
		}

		m_P1 = b2TransformPoint(m_Transform, m_Capsule.center1);
		m_P2 = b2TransformPoint(m_Transform, m_Capsule.center2);

	}

	float m_JumpStartSpeed;
	float m_JumpVariableSpeed;
	int m_JumpVariableTicks;
	int m_JumpBufferTicks;
	int m_JumpCoyoteTimeTicks;

	float m_WallJumpStartSpeed;
	float m_WallJumpVariableSpeed;
	float m_WallJumpStartSideSpeed;
	float m_WallJumpVariableSideSpeed;
	int   m_WallJumpVariableTicks;
	int   m_WallJumpBufferTicks;

	float m_DoubleJumpStartSpeed;
	float m_DoubleJumpVariableSpeed;
	int m_DoubleJumpVariableTicks;
	float m_DoubleJumpRayModifierForRegular;
	// ^^^ this value regulates the length of the ray for raycast that disables double jump near ground, the length is relative to m_DoubleJumpVariableTicks and current m_Velocity.y
	float m_DoubleJumpRayModifierForWall;
	// ^^^ same but for wall jumps
	
	float m_MaxSpeed;
	float m_MinSpeed;
	float m_StopSpeed;
	float m_Acceleration;
	float m_AirSteer;
	float m_Friction;
	float m_GravityDefault;
	float m_FastFallGravityModifier;
	float m_PogoHertz;
	float m_PogoDampingRatio;
	float m_PogoLengthScale;
	float m_SegmentOffset;
	float m_WallSlideSpeed;

	float originToBottom;

private:
	static constexpr int m_PlaneCapacity = 8;

	Cori::Physics::WorldRef m_World;
	Cori::Physics::BodyRef m_SensorVisitorBody;
	Cori::Physics::Capsule m_Capsule;
	Cori::Physics::Segment m_Segment;

	Cori::Physics::CastResult m_CastResult;
	Cori::Physics::CollisionPlane m_Planes[m_PlaneCapacity] = {};

	Cori::Physics::Transform m_Transform;
	Cori::Physics::Transform m_OldTransform;
	Cori::Physics::Vec2 m_Translation;
	Cori::Physics::Vec2 m_Velocity{ 0.0f, 0.0f };
	Cori::Physics::Vec2 m_Origin{ 0.0f, 0.0f };
	Cori::Physics::Vec2 m_P1;
	Cori::Physics::Vec2 m_P2;

	Cori::Physics::Vec2 m_GroundRayStart;
	Cori::Physics::Vec2 m_GroundRayEnd;

	Cori::Physics::Vec2 m_WallRayStart;
	Cori::Physics::Vec2 m_WallRayEnd;

	int m_PlaneCount{ 0 };
	int m_TotalIterations{ 0 };
	float m_PogoVelocity{ 0.0f };
	float m_Gravity{ 0.0f };

	bool m_OnGround{ false };
	bool m_OldOnGround{ false };
	bool m_JumpButtonReleased{ true };
	bool m_Jumping{ false };
	bool m_WallJumping{ false };
	bool m_DoubleJumping{ false };

	bool m_CanWallJump{ false };
	bool m_CanDoubleJump{ false };
	int m_WallJumpDirection{ 0 };


	int m_JumpVariableTickTimer{ 128 };
	int m_JumpBufferTickTimer{ 128 };
	int m_JumpCoyoteTimeTickTimer{ 128 };
	
	int m_WallJumpBufferTickTimer{ 128 };
	int m_WallJumpVariableTickTimer{ 128 };

	int m_DoubleJumpVariableTickTimer{ 128 };


	bool m_NearGround{ false };
	bool m_NearWall{ false };

	float test{ 1.0f };

	Cori::Entity m_Player{};
};