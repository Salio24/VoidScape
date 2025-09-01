#include "LevelLayer.hpp"

#include "Player/States.hpp"
#include "Tags.hpp"
#include "Player/AnimationPacks.hpp"
#include "EntityNameDefs.hpp"

static bool manualStep = false;

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {
	ActiveScene.DestroyEntity(m_Player);
}

void LevelLayer::OnAttach() {
	int screenWidth = Cori::Application::GetWindow().GetWidth();
	int screenHeight = Cori::Application::GetWindow().GetHeight();

	ActiveScene.GetActiveCamera().CreateOrthoCamera(0, static_cast<float>(screenWidth) / (screenHeight / 360.0f), 0, 360, -50, 0);

	m_MainCamera.SetWorldBound({{16, 16}, {1264, 784}});

	// player creation
	m_Player = ActiveScene.CreateEntity(EntityNames::PlayerRoot, Tags::Character);

	Cori::AssetManager::PreloadAnimationPacks({AnimationPacks::PlayerMovement, AnimationPacks::PlayerMovementFX});

	m_Player.AddComponent<Cori::Components::Entity::QuadRenderer>();
	m_Player.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(m_Player);
	m_Player.AddComponent<Cori::Components::Entity::Spawnpoint>(glm::vec2{ 160.0f, 160.0f });
	{
		auto& transform = m_Player.GetComponents<Cori::Components::Entity::Transform>();
		transform.SetLocalDepth(4);
	}

	Cori::Entity playerParticles1 = ActiveScene.CreateEntity("Movement Particles Part 1", Tags::Character);
	playerParticles1.AddComponent<Cori::Components::Entity::QuadRenderer>();
	playerParticles1.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(playerParticles1);
	CORI_CHECK_EXPECTED(playerParticles1.SetParent(m_Player));
	playerParticles1.SetActive(false);
	{
		auto& transform = playerParticles1.GetComponents<Cori::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	Cori::Entity playerParticles2 = ActiveScene.CreateEntity("Movement Particles Part 2", Tags::Character);
	playerParticles2.AddComponent<Cori::Components::Entity::QuadRenderer>();
	playerParticles2.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(playerParticles2);
	CORI_CHECK_EXPECTED(playerParticles2.SetParent(m_Player));
	playerParticles2.SetActive(false);
	{
		auto& transform = playerParticles2.GetComponents<Cori::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	Cori::Entity playerParticlesIndependent = ActiveScene.CreateEntity("Movement Particles Independent Root", Tags::Character);
	CORI_CHECK_EXPECTED(playerParticlesIndependent.SetParent(m_Player));
	{
		auto& transform = playerParticlesIndependent.GetComponents<Cori::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	{
		Cori::Entity particles = ActiveScene.CreateEntity("Landing Particles", Tags::Character);
		particles.AddComponent<Cori::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::Entity particles = ActiveScene.CreateEntity("Jumping Particles", Tags::Character);
		particles.AddComponent<Cori::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::Entity particles = ActiveScene.CreateEntity("WallJump Particles", Tags::Character);
		particles.AddComponent<Cori::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::Entity particles = ActiveScene.CreateEntity("DoubleJump Particles", Tags::Character);
		particles.AddComponent<Cori::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::Components::Entity::QuadAnimatorNew>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}


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
	fsm.SetState<States::Player::Idle>();

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
	//Cori::Renderer2D::SubmitScreenSpaceColoredQuad(ActiveScene.GetActiveCamera().GetSize() / 2.0f, {0.2f, 100}, {1, 1, 1});
	//Cori::Renderer2D::SubmitScreenSpaceColoredQuad(ActiveScene.GetActiveCamera().GetSize() / 2.0f, {100, 0.2f}, {1, 1, 1});

	Cori::Renderer2D::Test();

	m_Mover->OnUpdate(gameTimer.GetDeltaTime(), gameTimer.GetTickAlpha());
	m_MainCamera.OnUpdate(gameTimer, ActiveScene.GetActiveCamera());
}

void LevelLayer::OnTickUpdate(const float timeStep) {
	m_Mover->OnTickUpdate(timeStep, m_MainCamera);

	glm::vec2 playerPos = m_Player.GetComponents<Cori::Components::Entity::Transform>().GetLocalPosition();
	glm::vec2 playerHalfSize = m_Player.GetComponents<Cori::Components::Entity::QuadRenderer>().GetHalfSize();
	m_MainCamera.OnTickUpdate(timeStep, playerPos, playerHalfSize, Cori::Physics::ToPixels(m_Mover->m_Velocity) ,ActiveScene.GetActiveCamera());
}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw(ActiveScene.GetActiveCamera().GetSize(), CORI_PIXELS_PER_METER, this, true, ActiveScene.GetActiveCamera().GetPosition(), 2000.0f);
	}

	ImGui::Begin("Layer Layer UI");

	if (ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw)) {
		if (!m_PhysicsDebugDraw) {
			m_MoverDebugDraw = false;
		}
	}
	if (m_PhysicsDebugDraw) {
		ImGui::Checkbox("Mover debug draw", &m_MoverDebugDraw);
	}
	if (ImGui::Checkbox("Manual Step(disable, K - step)", &manualStep)) {
		Cori::Application::SetManualTickStep(manualStep);
	}

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

	static float scale = 1.0f;

	ImGui::Separator();

	if (ImGui::SliderFloat("Camera Scale", &scale, -0.25, 4.0f, "%.2f")) {
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
	Cori::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<Cori::WindowResizeEvent>([this](const Cori::WindowResizeEvent& e) -> bool {
		ActiveScene.GetActiveCamera().CreateOrthoCamera(0, static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight() / 360.0f), 0, 360, -50, 0);
			return true;
		});

}

