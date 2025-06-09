#include "GameLayer.hpp"
#include "LevelLayer.hpp"
#include "LevelLoader.hpp"

GameLayer::GameLayer() : Cori::Layer("Game Layer") {

}

GameLayer::~GameLayer() {

}

void GameLayer::OnAttach() {

}

void GameLayer::OnDetach() {

}

void GameLayer::OnUpdate(const double deltaTime) {

}

void GameLayer::OnTickUpdate() {

}

void GameLayer::OnImGuiRender(const double deltaTime) {
	ImGui::Begin("Game Layer UI");

	if (ImGui::Button("Test")) {
		Layer* level = new LevelLayer();
		Cori::SceneManager::CreateScene("Test Level");
		level->BindScene("Test Level");
		level->ActiveScene->ActiveCamera.CreateOrthoCamera(0, 7680, 0, 4320);
		LevelLoader::LoadLevel(level->ActiveScene, "../../assets/levels/testlevel.tmx");
		Cori::Application::PushLayer(level);
	}

	ImGui::End();


}

void GameLayer::OnEvent(Cori::Event& event) {

}
