#pragma once
#include <Cori.hpp>

struct ShapeUserData
{
	float maxPush;
	bool clipVelocity;
};

enum CollisionBits : uint64_t
{
	StaticBit = 1 << 0,
	MoverBit = 1 << 1,
	DynamicBit = 1 << 2,
	DebrisBit = 1 << 3,
	SensorBit = 1 << 4,

	AllBits = ~0u,
};

enum PogoShape
{
	PogoPoint,
	PogoCircle,
	PogoSegment
};

struct CastResult
{
	b2Vec2 point;
	b2Vec2 normal;
	b2BodyId bodyId;
	float fraction;
	bool hit;
};

static float CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
	CastResult* result = (CastResult*)context;
	result->point = point;
	result->normal = normal;
	result->bodyId = b2Shape_GetBody(shapeId);
	result->fraction = fraction;
	result->hit = true;
	return fraction;
}

class Mover {
public:
	static constexpr int m_planeCapacity = 8;
	static constexpr b2Vec2 m_elevatorBase = { 112.0f, 10.0f };
	static constexpr float m_elevatorAmplitude = 4.0f;

	float m_jumpSpeed = 10.0f;
	float m_maxSpeed = 6.0f;
	float m_minSpeed = 0.1f;
	float m_stopSpeed = 3.0f;
	float m_accelerate = 20.0f;
	float m_airSteer = 0.2f;
	float m_friction = 8.0f;
	float m_gravity = 30.0f;
	float m_pogoHertz = 5.0f;
	float m_pogoDampingRatio = 0.8f;

	int m_pogoShape = PogoSegment;
	b2Transform m_transform;
	b2Vec2 m_velocity;
	b2Capsule m_capsule;
	b2BodyId m_elevatorId;
	b2ShapeId m_ballId;
	ShapeUserData m_friendlyShape;
	ShapeUserData m_elevatorShape;
	b2CollisionPlane m_planes[m_planeCapacity] = {};
	int m_planeCount;
	int m_totalIterations;
	float m_pogoVelocity;
	float m_time;
	bool m_onGround;
	bool m_jumpReleased;
	bool m_lockCamera;

	b2Vec2 origin;
	b2Vec2 translation;
	b2Circle circle;
	b2Segment segment;
	CastResult castResult;

	Cori::Physics::BodyRef testCap{};

	b2Vec2 p1;
	b2Vec2 p2;

	Mover() {}

	void Init(const Cori::Layer* layer) {
		m_transform = { { 2.0f, 3.0f }, b2Rot_identity };
		m_velocity = { 0.0f, 0.0f };
		m_capsule = { { 0.0f, -0.5f }, { 0.0f, 0.5f }, 0.3f };

		m_totalIterations = 0;
		m_pogoVelocity = 0.0f;
		m_onGround = false;
		m_jumpReleased = true;
		m_lockCamera = true;
		m_planeCount = 0;
		m_time = 0.0f;

		auto ent = layer->ActiveScene->CreateEntity("Test cap");

		Cori::Physics::Body::Params bp;
		bp.type = b2_kinematicBody;
		bp.position = m_transform.p;
		bp.fixedRotation = true;
		bp.rotation = m_transform.q;

		testCap = ent.AddComponent<Cori::Components::Entity::Rigidbody>(layer->ActiveScene->PhysicsWorld, bp);
		
		Cori::Physics::Shape::Params sp;
		//sp.filter.categoryBits = CollisionBits::AllBits;
		sp.filter.maskBits = SensorBit;
		sp.isSensor = true;
		sp.enableSensorEvents = true;

		testCap.CreateShape(Cori::Physics::DestroyWithParent, sp, m_capsule);

	}

	void SolveMove(float timeStep, float throttle, const Cori::Layer* layer) {
		// Friction
		float speed = b2Length(m_velocity);
		if (speed < m_minSpeed)
		{
			m_velocity.x = 0.0f;
			m_velocity.y = 0.0f;
		}
		else if (m_onGround)
		{
			// Linear damping above stopSpeed and fixed reduction below stopSpeed
			float control = speed < m_stopSpeed ? m_stopSpeed : speed;

			// friction has units of 1/time
			float drop = control * m_friction * timeStep;
			float newSpeed = b2MaxFloat(0.0f, speed - drop);
			m_velocity *= newSpeed / speed;
		}

		b2Vec2 desiredVelocity = { m_maxSpeed * throttle, 0.0f };
		float desiredSpeed;
		b2Vec2 desiredDirection = b2GetLengthAndNormalize(&desiredSpeed, desiredVelocity);

		if (desiredSpeed > m_maxSpeed)
		{
			desiredSpeed = m_maxSpeed;
		}

		if (m_onGround)
		{
			m_velocity.y = 0.0f;
		}

		// Accelerate
		float currentSpeed = b2Dot(m_velocity, desiredDirection);
		float addSpeed = desiredSpeed - currentSpeed;
		if (addSpeed > 0.0f)
		{
			float steer = m_onGround ? 1.0f : m_airSteer;
			float accelSpeed = steer * m_accelerate * m_maxSpeed * timeStep;
			if (accelSpeed > addSpeed)
			{
				accelSpeed = addSpeed;
			}

			m_velocity += accelSpeed * desiredDirection;
		}

		m_velocity.y -= m_gravity * timeStep;

		float pogoRestLength = 3.0f * m_capsule.radius;
		float rayLength = pogoRestLength + m_capsule.radius;
		origin = b2TransformPoint(m_transform, m_capsule.center1);
		circle = { origin, 0.5f * m_capsule.radius };
		b2Vec2 segmentOffset = { 0.75f * m_capsule.radius, 0.0f };
		segment = {
			.point1 = origin - segmentOffset,
			.point2 = origin + segmentOffset,
		};

		b2ShapeProxy proxy = {};
		translation;
		b2QueryFilter pogoFilter = { MoverBit, StaticBit | DynamicBit };
		castResult = {};

		if (m_pogoShape == PogoPoint)
		{
			proxy = b2MakeProxy(&origin, 1, 0.0f);
			translation = { 0.0f, -rayLength };
		}
		else if (m_pogoShape == PogoCircle)
		{
			proxy = b2MakeProxy(&origin, 1, circle.radius);
			translation = { 0.0f, -rayLength + circle.radius };
		}
		else
		{
			proxy = b2MakeProxy(&segment.point1, 2, 0.0f);
			translation = { 0.0f, -rayLength };
		}

		b2World_CastShape(layer->ActiveScene->PhysicsWorld.Handle(), &proxy, translation, pogoFilter, CastCallback, &castResult);

		// Avoid snapping to ground if still going up
		if (m_onGround == false)
		{
			m_onGround = castResult.hit && m_velocity.y <= 0.01f;
		}
		else
		{
			m_onGround = castResult.hit;
		}

		if (castResult.hit == false)
		{

		}
		else
		{
			float pogoCurrentLength = castResult.fraction * rayLength;

			float offset = pogoCurrentLength - pogoRestLength;

			m_pogoVelocity = b2SpringDamper(m_pogoHertz, m_pogoDampingRatio, offset, m_pogoVelocity, timeStep);

			b2Body_ApplyForce(castResult.bodyId, { 0.0f, -50.0f }, castResult.point, true);
		}

		b2Vec2 target = m_transform.p + timeStep * m_velocity + timeStep * m_pogoVelocity * b2Vec2{ 0.0f, 1.0f };

		// Mover overlap filter
		b2QueryFilter collideFilter = { MoverBit, StaticBit | DynamicBit | MoverBit };

		// Movers don't sweep against other movers, allows for soft collision
		b2QueryFilter castFilter = { MoverBit, StaticBit | DynamicBit };

		m_totalIterations = 0;
		float tolerance = 0.01f;

		for (int iteration = 0; iteration < 5; ++iteration)
		{
			m_planeCount = 0;

			b2Capsule mover;
			mover.center1 = b2TransformPoint(m_transform, m_capsule.center1);
			mover.center2 = b2TransformPoint(m_transform, m_capsule.center2);
			mover.radius = m_capsule.radius;

			b2World_CollideMover(layer->ActiveScene->PhysicsWorld.Handle(), &mover, collideFilter, PlaneResultFcn, this);
			b2PlaneSolverResult result = b2SolvePlanes(target - m_transform.p, m_planes, m_planeCount);

			m_totalIterations += result.iterationCount;

			float fraction = b2World_CastMover(layer->ActiveScene->PhysicsWorld.Handle(), &mover, result.translation, castFilter);

			b2Vec2 delta = fraction * result.translation;
			m_transform.p += delta;

			if (b2LengthSquared(delta) < tolerance * tolerance)
			{
				break;
			}
		}

		m_velocity = b2ClipVector(m_velocity, m_planes, m_planeCount);
	}

	void UpdateGui() 
	{

		ImGui::Begin("Mover");

		ImGui::SliderFloat("Jump Speed", &m_jumpSpeed, 0.0f, 40.0f, "%.0f");
		ImGui::SliderFloat("Min Speed", &m_minSpeed, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Max Speed", &m_maxSpeed, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("Stop Speed", &m_stopSpeed, 0.0f, 10.0f, "%.1f");
		ImGui::SliderFloat("Accelerate", &m_accelerate, 0.0f, 100.0f, "%.0f");
		ImGui::SliderFloat("Friction", &m_friction, 0.0f, 10.0f, "%.1f");
		ImGui::SliderFloat("Gravity", &m_gravity, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("Air Steer", &m_airSteer, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Pogo Hertz", &m_pogoHertz, 0.0f, 30.0f, "%.0f");
		ImGui::SliderFloat("Pogo Damping", &m_pogoDampingRatio, 0.0f, 4.0f, "%.1f");

		ImGui::Separator();

		ImGui::Text("Pogo Shape");
		ImGui::RadioButton("Point", &m_pogoShape, PogoPoint);
		ImGui::SameLine();
		ImGui::RadioButton("Circle", &m_pogoShape, PogoCircle);
		ImGui::SameLine();
		ImGui::RadioButton("Segment", &m_pogoShape, PogoSegment);

		ImGui::Checkbox("Lock Camera", &m_lockCamera);

		ImGui::End();
	}

	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult* planeResult, void* context)
	{
		assert(planeResult->hit == true);

		Mover* self = static_cast<Mover*>(context);
		float maxPush = FLT_MAX;
		bool clipVelocity = true;
		ShapeUserData* userData = static_cast<ShapeUserData*>((void*)b2Shape_GetUserData(shapeId));
		if (userData != nullptr)
		{
			Cori::Physics::ShapeRef shape = Cori::Physics::ShapeRef(shapeId);

			maxPush = userData->maxPush;
			clipVelocity = userData->clipVelocity;
		}

		if (self->m_planeCount < m_planeCapacity)
		{
			assert(b2IsValidPlane(planeResult->plane));
			self->m_planes[self->m_planeCount] = { planeResult->plane, maxPush, 0.0f, clipVelocity };
			self->m_planeCount += 1;
		}

		return true;
	}

	static bool Kick(b2ShapeId shapeId, void* context)
	{
		Mover* self = (Mover*)context;
		b2BodyId bodyId = b2Shape_GetBody(shapeId);
		b2BodyType type = b2Body_GetType(bodyId);

		if (type != b2_dynamicBody)
		{
			return true;
		}

		b2Vec2 center = b2Body_GetWorldCenterOfMass(bodyId);
		b2Vec2 direction = b2Normalize(center - self->m_transform.p);
		b2Vec2 impulse = b2Vec2{ 2.0f * direction.x, 2.0f };
		b2Body_ApplyLinearImpulseToCenter(bodyId, impulse, true);

		return true;
	}

	void Keyboard(int key) 
	{

	}

	void Step(const float timeStep, const Cori::Layer* layer) 
	{

		float throttle = 0.0f;

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_A))
		{
			throttle -= 1.0f;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_D))
		{
			throttle += 1.0f;
		}

		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_SPACE))
		{
			if (m_onGround == true && m_jumpReleased)
			{
				m_velocity.y = m_jumpSpeed;
				m_onGround = false;
				m_jumpReleased = false;
			}
		}
		else
		{
			m_jumpReleased = true;
		}

		SolveMove(timeStep, throttle, layer);
		testCap.SetTargetTransform(m_transform, timeStep);
	
		int count = m_planeCount;

		//for (int i = 0; i < count; ++i) {
		//	b2Plane plane = m_planes[i].plane;
		//	b2Vec2 p1 = m_transform.p + (plane.offset - m_capsule.radius) * plane.normal;
		//	b2Vec2 p2 = p1 + 0.1f * plane.normal;
		//	layer->debug_renderer.DrawPoint(p1, 5.0f, b2_colorYellow);
		//	layer->debug_renderer.DrawLine(p1, p2, b2_colorYellow);
		//}

		p1 = b2TransformPoint(m_transform, m_capsule.center1);
		p2 = b2TransformPoint(m_transform, m_capsule.center2);

		b2Vec2 p = m_transform.p;

	}
	
	void DebugDraw(const Cori::Layer* layer) {
		if (castResult.hit == false)
		{
			m_pogoVelocity = 0.0f;

			b2Vec2 delta = translation;
			layer->debug_renderer.DrawLine(origin, origin + delta, b2_colorGray);

			if (m_pogoShape == PogoPoint)
			{
				layer->debug_renderer.DrawPoint(origin + delta, 10.0f, b2_colorGray);
			}
			else if (m_pogoShape == PogoCircle)
			{
				layer->debug_renderer.DrawCircle(origin + delta, circle.radius, b2_colorGray);
			}
			else
			{
				layer->debug_renderer.DrawLine(segment.point1 + delta, segment.point2 + delta, b2_colorGray);
			}
		}
		else
		{

			b2Vec2 delta = castResult.fraction * translation;
			layer->debug_renderer.DrawLine(origin, origin + delta, b2_colorGray);

			if (m_pogoShape == PogoPoint)
			{
				layer->debug_renderer.DrawPoint(origin + delta, 10.0f, b2_colorPlum);
			}
			else if (m_pogoShape == PogoCircle)
			{
				layer->debug_renderer.DrawCircle(origin + delta, circle.radius, b2_colorPlum);
			}
			else
			{
				layer->debug_renderer.DrawLine(segment.point1 + delta, segment.point2 + delta, b2_colorPlum);
			}
		}
		b2HexColor color = m_onGround ? b2_colorOrange : b2_colorAquamarine;
		layer->debug_renderer.DrawCapsuleFilled(p1, p2, m_capsule.radius, color);
		layer->debug_renderer.DrawLine(m_transform.p, m_transform.p + m_velocity, b2_colorPurple);
	}

};