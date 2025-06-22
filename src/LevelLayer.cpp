#include "LevelLayer.hpp"


LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {
	ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360);

	Mover::Params mp;
	mp.position = { 5.0f, 5.0f };

	m_Mover.reset(new Mover(Cori::Physics::Capsule::Create({ 0.0f, -0.5f }, { 0.0f, 0.5f }, 0.3f), ActiveScene->PhysicsWorld, mp));
}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const double deltaTime, const double tickAlpha) {

}

void LevelLayer::OnTickUpdate(const float timeStep) {
	m_Mover->OnTickUpdate(timeStep);
}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw({ 640, 360 }, CORI_PIXELS_PER_METER, this, true, 2000.0f);
	}

	ImGui::Begin("Layer Layer UI");

	ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw);
	ImGui::Checkbox("Mover debug draw", &m_MoverDebugDraw);


	if (ImGui::Button("Add b2Box")) {
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
		m_Mover->DebugDraw();
		m_Mover->UpdateGui();
	}

}

void LevelLayer::OnEvent(Cori::Event& event) {

}

