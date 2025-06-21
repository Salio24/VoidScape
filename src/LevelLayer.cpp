#include "LevelLayer.hpp"


LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {
	ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360);

	auto ent = ActiveScene->CreateEntity("Floor");

	Cori::Physics::Body::Params bp;
	bp.type = b2_staticBody;
	bp.position = { 0.0f, 0.0f };

	auto& rb = ent.AddComponent<Cori::Components::Entity::Rigidbody>(ActiveScene->PhysicsWorld, bp);

	Cori::Physics::Shape::Params sp;

	rb.CreateShape(Cori::Physics::DestroyWithParent, sp, Cori::Physics::Polygon::CreateBox({ 40.0f, 1.0f }));

	mover.Init(this);
}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const double deltaTime) {

}

void LevelLayer::OnTickUpdate(const float timeStep) {
	mover.Step(timeStep, this);
}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw({ 640, 360 }, CORI_PIXELS_PER_METER, this, true, 2000.0f);
	}

	ImGui::Begin("Layer Layer UI");

	ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw);

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

	mover.DebugDraw(this);
	mover.UpdateGui();

}

void LevelLayer::OnEvent(Cori::Event& event) {

}

