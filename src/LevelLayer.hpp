#pragma once
#include <Cori.hpp>
#include "Player/Mover.hpp"
#include "Triggers/TriggerScripts.hpp"
#include "Components.hpp"

class LevelLayer : public Cori::Layer {
public:
	LevelLayer();

	~LevelLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const Cori::GameTimer& gameTimer) override;
	void OnTickUpdate(const float timeStep) override;
	void OnImGuiRender(const double deltaTime) override;

	void OnEvent(Cori::Event& event) override;

private:
	bool m_PhysicsDebugDraw{ false };
	bool m_MoverDebugDraw{ false };

	Cori::Entity m_Player;

	MainCamera m_MainCamera;

	std::unique_ptr<Mover> m_Mover;
};
