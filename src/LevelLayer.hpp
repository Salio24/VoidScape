#pragma once
#include <Cori.hpp>
#include "Player/Mover.hpp"

class LevelLayer final : public Cori::Core::Layer {
public:
	LevelLayer();

	~LevelLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Cori::Core::GameTimer& gameTimer) override;
	void OnTickUpdate(Cori::Core::GameTimer& gameTimer) override;
	void OnImGuiRender(Cori::Core::GameTimer& gameTimer) override;

	void OnEvent(Cori::Core::Event& event) override;

	void LoadLevel(const std::filesystem::path& path);

private:
	void CreatePlayer(const float startingTime, const glm::vec2 spawnPos);

	void CreateEscapeDoor(const Cori::Physics::Vec2 pos);

	void AddRegularOrb(const float orbBonus, const Cori::Physics::Vec2 pos);

	bool m_LevelLoaded{ false };
	bool m_LevelCompleted{ false };
	bool m_PhysicsDebugDraw{ false };
	bool m_MoverDebugDraw{ false };

	Cori::World::Entity m_Player;

	MainCamera m_MainCamera;
};
