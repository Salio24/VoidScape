#include "LevelLayer.hpp"
#include "PlayerStates.hpp"

static bool manualStep = false;

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {
	ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360);

	m_Player = ActiveScene->CreateEntity("Player");

	auto& an = m_Player.AddComponent<Cori::Animator>("../../assets/player/PlayerSheet.json", m_Player, (1.0f / 60.0f), "Test");

	auto& rend = m_Player.AddComponent<Cori::Components::Entity::Render>();
	rend.m_Position = { 100.0f, 200.0f };
	rend.m_Size = an.m_FrameSize;

	// maybe add placeholder as a default to ctor?
	auto& sp = m_Player.AddComponent<Cori::Components::Entity::Sprite>();
	sp.m_Texture = Cori::AssetManager::GetTexture2D(Cori::Texture2Ds::Placeholder);
	sp.m_UVs.UVmin = { 0.0f, 0.0f };
	sp.m_UVs.UVmax = { 1.0f, 1.0f };

	auto& fsm = m_Player.AddComponent <Cori::Components::Entity::StateMachine>(m_Player);

	fsm.Register<PlayerStates::IdleState>();
	fsm.SetState<PlayerStates::IdleState>();
	fsm.Register<PlayerStates::RunState>();
	fsm.Register<PlayerStates::FallState>();
	fsm.Register<PlayerStates::JumpState>();
	fsm.Register<PlayerStates::DoubleJumpState>();
	fsm.Register<PlayerStates::WallJumpState>();
	fsm.Register<PlayerStates::WallSlideState>();

	Mover::Params mp;
	mp.position = { 5.0f, 5.0f };
	mp.gravityDefault = 34.5f;
	m_Mover.reset(new Mover(Cori::Physics::Capsule::Create({ 0.0f, -0.5f }, { 0.0f, 0.7f }, 0.37f), ActiveScene->PhysicsWorld, m_Player, mp));
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
	
		auto& rb = ent.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene->PhysicsWorld, bp);
	
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

