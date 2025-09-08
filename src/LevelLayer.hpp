#pragma once
#include <Cori.hpp>
#include "Player/Mover.hpp"
#include "Triggers/TriggerScripts.hpp"
#include "Components.hpp"

class LevelLayer final : public Cori::Core::Layer {
public:
	LevelLayer();

	~LevelLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const Cori::Core::GameTimer& gameTimer) override;
	void OnTickUpdate(const float timeStep) override;
	void OnImGuiRender(const double deltaTime) override;

	void OnEvent(Cori::Core::Event& event) override;

private:
	bool m_PhysicsDebugDraw{ false };
	bool m_MoverDebugDraw{ false };

	Cori::World::Entity m_Player;

	MainCamera m_MainCamera;

	std::unique_ptr<Mover> m_Mover;
};
