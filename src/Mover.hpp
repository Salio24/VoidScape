#pragma once
#include <Cori.hpp>


class Mover {
public:

	struct Params {
		Params() = default;

		Cori::Physics::Vec2 position{ 0.0f, 0.0f };
		float jumpSpeed{ 10.0f };
		float maxSpeed{ 6.0f };
		float minSpeed{ 0.1f };
		float stopSpeed{ 3.0f };
		float acceleration{ 20.0f };
		float airSteer{ 0.2f };
		float friction{ 8.0f };
		float gravity{ 30.0f };
		float pogoHertz{ 5.0f };
		float pogoDampingRatio{ 0.8f };
		float pogoLengthScale{ 2.0f };
		float segmentOffset{ 0.9f };
	};

	Mover(const Cori::Physics::Capsule& capsule, Cori::Physics::WorldRef world, const Params& def) : m_Capsule(capsule), m_World(world), m_Transform{ def.position, b2Rot_identity }, 
			m_JumpSpeed(def.jumpSpeed), m_MaxSpeed(def.maxSpeed), m_MinSpeed(def.minSpeed), m_StopSpeed(def.stopSpeed), m_Acceleration(def.acceleration), m_AirSteer(def.airSteer), 
			m_Friction(def.friction), m_Gravity(def.gravity), m_PogoHertz(def.pogoHertz), m_PogoDampingRatio(def.pogoDampingRatio), m_PogoLengthScale(def.pogoLengthScale), m_SegmentOffset(def.segmentOffset) {
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

		//Vec2

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
		float tolerance = 0.01f;

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

	void UpdateGui() {
		ImGui::Begin("Mover");

		ImGui::SliderFloat("Jump Speed", &m_JumpSpeed, 0.0f, 40.0f, "%.0f");
		ImGui::SliderFloat("Min Speed", &m_MinSpeed, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Max Speed", &m_MaxSpeed, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("Stop Speed", &m_StopSpeed, 0.0f, 40.0f, "%.1f");
		ImGui::SliderFloat("Accelerate", &m_Acceleration, 0.0f, 100.0f, "%.0f");
		ImGui::SliderFloat("Friction", &m_Friction, 0.0f, 40.0f, "%.1f");
		ImGui::SliderFloat("Gravity", &m_Gravity, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("Air Steer", &m_AirSteer, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Pogo Hertz", &m_PogoHertz, 0.0f, 30.0f, "%.0f");
		ImGui::SliderFloat("Pogo Damping", &m_PogoDampingRatio, 0.0f, 4.0f, "%.1f");
		ImGui::SliderFloat("Pogo Length Scale", &m_PogoLengthScale, 0.1f, 10.0f, "%.2f");
		ImGui::SliderFloat("Segment Offset", &m_SegmentOffset, 0.1f, 4.0f, "%.2f");

		ImGui::End();
	}

	void DebugDraw() {
		if (m_CastResult.hit == false) {
			b2Vec2 delta = m_Translation;
			Cori::Layer::debug_renderer.DrawLine(m_Origin, m_Origin + delta, b2_colorPurple);
			Cori::Layer::debug_renderer.DrawLine(m_Segment.point1 + delta, m_Segment.point2 + delta, b2_colorPurple);
		}
		else {
			b2Vec2 delta = m_CastResult.fraction * m_Translation;
			Cori::Layer::debug_renderer.DrawLine(m_Origin, m_Origin + delta, b2_colorPurple);
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
	}

	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult* planeResult, void* context) {
		assert(planeResult->hit == true);

		Mover* self = static_cast<Mover*>(context);

		if (self->m_PlaneCount < m_PlaneCapacity) {
			assert(b2IsValidPlane(planeResult->plane));
			self->m_Planes[self->m_PlaneCount] = { planeResult->plane, FLT_MAX, 0.0f, true };
			self->m_PlaneCount += 1;
		}

		return true;
	}

	void OnUpdate(const double deltaTime, const double tickAlpha) {

	}

	void OnTickUpdate(const float timeStep) {
		float throttle = 0.0f;

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_A)) {
			throttle -= 1.0f;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_D)) {
			throttle += 1.0f;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_SPACE)) {
			if (m_OnGround == true && m_JumpReleased) {
				m_Velocity.y = m_JumpSpeed;
				m_OnGround = false;
				m_JumpReleased = false;
			}
		}
		else {
			m_JumpReleased = true;
		}

		constexpr float tolerance = 0.01f;

		m_OldTransform = m_Transform;

		SolveMove(timeStep, throttle);

		b2Vec2 delta = m_OldTransform.p - m_Transform.p;

		if (std::abs(delta.x) > tolerance || std::abs(delta.y) > tolerance) {
			m_SensorVisitorBody.SetTargetTransform(m_Transform, timeStep);
		}
		else {
			m_SensorVisitorBody.SetLinearVelocity({ 0.0f, 0.0f });
		}

		m_P1 = b2TransformPoint(m_Transform, m_Capsule.center1);
		m_P2 = b2TransformPoint(m_Transform, m_Capsule.center2);
	}

	float m_JumpSpeed = 10.0f;
	float m_MaxSpeed = 6.0f;
	float m_MinSpeed = 0.1f;
	float m_StopSpeed = 3.0f;
	float m_Acceleration = 20.0f;
	float m_AirSteer = 0.2f;
	float m_Friction = 8.0f;
	float m_Gravity = 30.0f;
	float m_PogoHertz = 5.0f;
	float m_PogoDampingRatio = 0.8f;
	float m_PogoLengthScale = 3.0f;
	float m_SegmentOffset = 0.75f;

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

	int m_PlaneCount{ 0 };
	int m_TotalIterations{ 0 };
	float m_PogoVelocity{ 0.0f };

	bool m_OnGround{ false };
	bool m_JumpReleased{ true };
	bool m_LockCamera{ true };

	Cori::Entity m_Player{};
};