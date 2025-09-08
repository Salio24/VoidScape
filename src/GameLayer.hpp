#pragma once
#include <Cori.hpp>

class GameLayer final : public Cori::Core::Layer {
public:
	GameLayer();

	~GameLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const Cori::Core::GameTimer& gameTimer) override;
	void OnTickUpdate(const float timeStep) override;
	void OnImGuiRender(const double deltaTime) override;

	void OnEvent(Cori::Core::Event& event) override;
};
