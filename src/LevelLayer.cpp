#include "LevelLayer.hpp"

#include <memory>
#include "Player/States.hpp"
#include "Tags.hpp"

static bool manualStep = false;

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {
	ActiveScene.DestroyEntity(m_Player);
}

void LevelLayer::OnAttach() {
	ActiveScene.GetActiveCamera().CreateOrthoCamera(0, 640, 0, 360, -50, 0);
	m_MainCamera.SetWorldBound({{-320.0f, -180.0f}, {30000.0f, 30000.0f}});
	m_MainCamera.SetRenderCameraSize({640.0f, 360.0f});

	// player creation
	m_Player = ActiveScene.CreateEntity("Player Root", Tags::Character);

	auto& renderer = m_Player.AddComponent<Cori::Components::Entity::QuadRenderer>();
	auto& animator = m_Player.AddComponent<Cori::Components::Entity::QuadAnimator>("../../assets/player/PlayerSheet.json", m_Player, (1.0f / 60.0f), "Test");
	auto& spawn = m_Player.AddComponent<Cori::Components::Entity::Spawnpoint>(glm::vec2{ 80.0f, 80.0f });
	auto& transform = m_Player.GetComponents<Cori::Components::Entity::Transform>();
	transform.SetLocalDepth(2);

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
	mp.gravityDefault = 34.5f;
	m_Mover = std::make_unique<Mover>(Cori::Physics::Capsule::Create({ 0.0f, -0.5f }, { 0.0f, 0.55f }, 0.37f), ActiveScene.GetPhysicsWorld(), m_Player, mp);

	// test trigger creation
	auto tr = ActiveScene.CreateEntity("TestTrigger", Tags::Triggers);
	
	Cori::Physics::Body::Params bp;
	bp.type = b2_staticBody;
	bp.position = { 2.0f, 5.0f };
	bp.name = "TestTrigger";

	auto& rb = tr.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene.GetPhysicsWorld(), bp, tr);

	Cori::Physics::Shape::Params spa;
	spa.filter.categoryBits = Cori::Physics::CollisionBits::SensorBit;
	spa.isSensor = true;
	spa.enableSensorEvents = true;

	rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Polygon::CreateBox({ 2.0f, 2.0f }));
	//rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Circle::Create({ 5.0f, 5.0f }, 2.0f));

	auto& trig = tr.AddComponent<Cori::Components::Entity::Trigger>(tr);
	trig.SetBehavior<TestTrigger>();
}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const Cori::GameTimer& gameTimer) {
	Cori::Renderer2D::SubmitScreenSpaceColoredQuad({320, 180}, {0.2f, 100}, {1, 1, 1});
	Cori::Renderer2D::SubmitScreenSpaceColoredQuad({320, 180}, {100, 0.2f}, {1, 1, 1});

	m_Mover->OnUpdate(gameTimer.GetDeltaTime(), gameTimer.GetTickAlpha());
	m_MainCamera.OnUpdate(gameTimer, ActiveScene.GetActiveCamera());
}

void LevelLayer::OnTickUpdate(const float timeStep) {
	if (!manualStep) {
		m_Mover->OnTickUpdate(timeStep, m_MainCamera);
	}
	else {
		static bool oneshot = true;
		if (Cori::Input::IsKeyPressed(Cori::CORI_KEY_K)) {
			if (oneshot) {
				m_Mover->OnTickUpdate(timeStep, m_MainCamera);
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

	glm::vec2 playerPos = m_Player.GetComponents<Cori::Components::Entity::Transform>().GetLocalPosition();
	glm::vec2 playerHalfSize = m_Player.GetComponents<Cori::Components::Entity::QuadRenderer>().m_HalfSize;
	m_MainCamera.OnTickUpdate(timeStep, playerPos, playerHalfSize, Cori::Physics::ToPixels(m_Mover->m_Velocity) ,ActiveScene.GetActiveCamera());
}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	static int camim = 0.0f;
	static int camgl = 0.0f;
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw({ 640, 360 }, CORI_PIXELS_PER_METER, this, true, ActiveScene.GetActiveCamera().GetPosition(), 2000.0f);
	}

	//ActiveScene.GetActiveCamera().SetPosition({camgl, camgl});
	//ActiveScene.GetActiveCamera().RecalculateVP();

	ImGui::Begin("Layer Layer UI");

	ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw);
	ImGui::Checkbox("Mover debug draw", &m_MoverDebugDraw);
	ImGui::Checkbox("Manual Step(J - enable/disable, K - step)", &manualStep);

	if (ImGui::Button("Add dynamic box")) {
		auto ent = ActiveScene.CreateEntity("Dynamic Box", Tags::ForTest);
	
		Cori::Physics::Body::Params bp;
		bp.type = b2_dynamicBody;
		bp.position = { 4.0f, 4.0f };
	
		auto& rb = ent.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene.GetPhysicsWorld(), bp, ent);
	
		Cori::Physics::Shape::Params sp;
	
		rb.CreateShape(Cori::Physics::DestroyWithParent, sp, Cori::Physics::Polygon::CreateBox({ 1.0f, 1.0f }));
	}

	ImGui::SeparatorText("Camera Settings");

	ImGui::SliderFloat("Asymptotic Average X", &m_MainCamera.m_AsymptoticAverageX, 0.01f, 1.0f, "%.2f");
	ImGui::SliderFloat("Asymptotic Average Y", &m_MainCamera.m_AsymptoticAverageY, 0.01f, 1.0f, "%.2f");
	ImGui::SliderFloat("Velocity Dep Mod x", &m_MainCamera.m_VelocityDependencyModifierX, -1.0f, 1.0f, "%.2f");
	ImGui::SliderFloat("Velocity Dep Mod Y", &m_MainCamera.m_VelocityDependencyModifierY, -1.0f, 1.0f, "%.2f");
	ImGui::SliderFloat("m_MaxTransformShakeX", &m_MainCamera.m_MaxTransformShakeX, 0.0f, 1000.0f, "%.2f");
	ImGui::SliderFloat("m_MaxTransformShakeY", &m_MainCamera.m_MaxTransformShakeY, 0.0f, 1000.0f, "%.2f");
	ImGui::SliderFloat("m_MaxRotationalShake", &m_MainCamera.m_MaxRotationalShake, 0.0f, 1000.0f, "%.2f");
	ImGui::SliderFloat("m_TransformShakeFrequencyModifier", &m_MainCamera.m_TransformShakeFrequencyModifier, 0.01f, 2.0f, "%.2f");
	ImGui::SliderFloat("m_RotationalShakeFrequencyModifier", &m_MainCamera.m_RotationalShakeFrequencyModifier, 0.01f, 2.0f, "%.2f");


	static int rot = 0;

	static float scale = 1.0f;

	ImGui::Separator();

	if (ImGui::SliderInt("Rotation", &rot, -360, 360, "%.0d")) {
		ActiveScene.GetActiveCamera().SetRotation(rot);
		ActiveScene.GetActiveCamera().RecalculateVP();
	}
	if (ImGui::SliderFloat("Scale", &scale, -0.25, 4.0f, "%.2f")) {
		ActiveScene.GetActiveCamera().SetZoomLevel(scale);
		ActiveScene.GetActiveCamera().RecalculateVP();
	}
	if (ImGui::Button("Add 0.5 trauma")) {
		m_MainCamera.AddTrauma(0.5f);
	}

	if (ImGui::Button("Add 0.1 trauma")) {
		m_MainCamera.AddTrauma(0.1f);
	}

	ImGui::End();

	if (m_MoverDebugDraw) {
		m_Mover->DebugDraw(static_cast<float>(deltaTime));
		m_Mover->UpdateGui();
	}

}

void LevelLayer::OnEvent(Cori::Event& event) {

}

