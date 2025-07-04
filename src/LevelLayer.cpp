#include "LevelLayer.hpp"
#include "Player/States.hpp"

static bool manualStep = false;

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {
	ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360, -10, 0);

	// player creation
	m_Player = ActiveScene->CreateEntity("Player");

	auto& an = m_Player.AddComponent<Cori::Components::Entity::Animator>("../../assets/player/PlayerSheet.json", m_Player, (1.0f / 60.0f), "Test");

	auto& spawn = m_Player.AddComponent<Cori::Components::Entity::Spawnpoint>(glm::vec2{ 80.0f, 80.0f });

	auto& rend = m_Player.AddComponent<Cori::Components::Entity::Render>();
	rend.m_Position = spawn.m_Spawnpoint;
	rend.m_Size = an.m_FrameSize;
	rend.m_Layer = 2.0f;

	auto& sp = m_Player.AddComponent<Cori::Components::Entity::Sprite>();
	sp.m_Texture = Cori::AssetManager::GetTexture2D(Cori::Texture2Ds::Placeholder);
	sp.m_UVs.UVmin = { 0.0f, 0.0f };
	sp.m_UVs.UVmax = { 1.0f, 1.0f };

	auto& fsm = m_Player.AddComponent <Cori::Components::Entity::StateMachine>(m_Player);

	fsm.Register<States::Player::Idle>();
	fsm.SetState<States::Player::Idle>();
	fsm.Register<States::Player::Run>();
	fsm.Register<States::Player::Fall>();
	fsm.Register<States::Player::Jump>();
	fsm.Register<States::Player::DoubleJump>();
	fsm.Register<States::Player::WallJump>();
	fsm.Register<States::Player::WallSlide>();
	fsm.Register<States::Player::Ascending>();

	Mover::Params mp;
	//mp.position = { 5.0f, 5.0f };
	mp.gravityDefault = 34.5f;
	m_Mover.reset(new Mover(Cori::Physics::Capsule::Create({ 0.0f, -0.5f }, { 0.0f, 0.55f }, 0.37f), ActiveScene->PhysicsWorld, m_Player, mp));

	// test trigger creation
	auto tr = ActiveScene->CreateEntity("TestTrigger");
	
	Cori::Physics::Body::Params bp;
	bp.type = b2_staticBody;
	bp.position = { 0.0f, 5.0f };
	bp.name = "TestTrigger";

	auto& rb = tr.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene->PhysicsWorld, bp, tr);

	Cori::Physics::Shape::Params spa;
	spa.filter.categoryBits = Cori::Physics::CollisionBits::SensorBit;
	spa.isSensor = true;
	spa.enableSensorEvents = true;

	rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Polygon::CreateBox({ 5.0f, 5.0f }));
	//rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Circle::Create({ 5.0f, 5.0f }, 2.0f));

	auto& trig = tr.AddComponent<Cori::Components::Entity::Trigger>(tr);
	trig.SetBehavior<TestTrigger>();
}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const double deltaTime, const double tickAlpha) {
	m_Mover->OnUpdate(deltaTime, tickAlpha);
}

void LevelLayer::OnTickUpdate(const float timeStep) {
	if (!manualStep) {
		m_Mover->OnTickUpdate(timeStep);
	}
	else {
		static bool oneshot = true;
		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_K)) {
			if (oneshot) {
				m_Mover->OnTickUpdate(timeStep);
				oneshot = false;
			}
		}
		else {
			oneshot = true;
		}
	}

	static bool oneshot2 = true;

	if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_J)) {
		if (oneshot2) {
			manualStep = !manualStep;
			oneshot2 = false;
		}
	}
	else {
		oneshot2 = true;
	}
}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw({ 640, 360 }, CORI_PIXELS_PER_METER, this, true, 2000.0f);
	}

	ImGui::Begin("Layer Layer UI");

	ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw);
	ImGui::Checkbox("Mover debug draw", &m_MoverDebugDraw);
	ImGui::Checkbox("Manual Step(J - enable/disable, K - step)", &manualStep);

	if (ImGui::Button("Add dynamic box")) {
		auto ent = ActiveScene->CreateEntity();
	
		Cori::Physics::Body::Params bp;
		bp.type = b2_dynamicBody;
		bp.position = { 4.0f, 4.0f };
	
		auto& rb = ent.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene->PhysicsWorld, bp, ent);
	
		Cori::Physics::Shape::Params sp;
	
		rb.CreateShape(Cori::Physics::DestroyWithParent, sp, Cori::Physics::Polygon::CreateBox({ 1.0f, 1.0f }));
	}

	ImGui::End();

	if (m_MoverDebugDraw) {
		m_Mover->DebugDraw((float)deltaTime);
		m_Mover->UpdateGui();
	}

}

void LevelLayer::OnEvent(Cori::Event& event) {

}

