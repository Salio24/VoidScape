#include "LevelLayer.hpp"

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {
	ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360);
}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const double deltaTime) {

}

void LevelLayer::OnTickUpdate(const float timeStep) {

}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	if (m_PhysicsDebugDraw) {
		Cori::ImGuiPresets::Box2dDebugDraw({ 640, 360 }, CORI_PIXELS_PER_METER, this, true, 2000.0f);
	}

	ImGui::Begin("Layer Layer UI");

	ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw);


	ImGui::End();



}

void LevelLayer::OnEvent(Cori::Event& event) {

}

