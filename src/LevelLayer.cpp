#include "LevelLayer.hpp"

LevelLayer::LevelLayer() : Cori::Layer("Level Layer") {

}

LevelLayer::~LevelLayer() {

}

void LevelLayer::OnAttach() {

}

void LevelLayer::OnDetach() {

}

void LevelLayer::OnUpdate(const double deltaTime) {
	ActiveScene->ActiveCamera.SetPosition(glm::vec2(sliderint, 0));
	ActiveScene->ActiveCamera.RecalculateVP();
}

void LevelLayer::OnTickUpdate() {

}

void LevelLayer::OnImGuiRender(const double deltaTime) {
	ImGui::Begin("Layer Layer UI");

	ImGui::SliderInt("slider int", &sliderint, -100, 100);
	ImGui::SliderFloat("slider float", &sliderfloat, -100, 100);


	ImGui::End();



}

void LevelLayer::OnEvent(Cori::Event& event) {

}

