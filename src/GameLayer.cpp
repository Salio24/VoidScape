#include "GameLayer.hpp"
#include "LevelLayer.hpp"
#include <PathDefinesGenerated.hpp>

GameLayer::GameLayer() : Layer("Game Layer") {

}

GameLayer::~GameLayer() {

}

void GameLayer::OnAttach() {
	auto level = new LevelLayer();

	level->LoadLevel(GetVoidScapeRootDir() / "assets/levels/testlevel.tmx");

	auto result = Cori::Core::Application::PushLayer(level);
	if (!result) {
		CORI_ERROR("Failed to push LevelLayer, Error: {}", result.error().what());
	}
}

void GameLayer::OnDetach() {

}

void GameLayer::OnUpdate(Cori::Core::GameTimer& gameTimer) {

}

void GameLayer::OnTickUpdate(Cori::Core::GameTimer& gameTimer) {

}

void GameLayer::OnImGuiRender(Cori::Core::GameTimer& gameTimer) {
	ImGui::Begin("Game Layer UI");

	ImGui::SeparatorText("Graphical Settings");

	Cori::ImGuiPresets::ScreenModeAndResolutionDropdowns();

	ImGui::End();


}

void GameLayer::OnEvent(Cori::Core::Event& event) {

}
