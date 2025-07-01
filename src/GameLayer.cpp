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

void GameLayer::OnUpdate(const double deltaTime, const double tickAlpha) {

}

void GameLayer::OnTickUpdate(const float timeStep) {

}

void GameLayer::OnImGuiRender(const double deltaTime) {
	ImGui::Begin("Game Layer UI");

	if (ImGui::Button("Start")) {
		Layer* level = new LevelLayer();
		if (Cori::SceneManager::CreateScene("Test Level")) {
			level->BindScene("Test Level");
			//level->ActiveScene->ActiveCamera.CreateOrthoCamera(0, 640, 0, 360);
			LevelLoader::LoadLevel(level->ActiveScene, "../../assets/levels/testlevel.tmx");
			//LevelLoader::LoadLevel(level->ActiveScene, "../../../Source/levels/GameLevels/32p/Level_1.tmx");
		}

		Cori::Application::PushLayer(level);
	}

	ImGui::End();


}

void GameLayer::OnEvent(Cori::Event& event) {

}
