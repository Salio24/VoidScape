#pragma once
#include <Cori.hpp>

class GameLayer : public Cori::Layer {
public:
	GameLayer();

	~GameLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const Cori::GameTimer& gameTimer) override;
	void OnTickUpdate(const float timeStep) override;
	void OnImGuiRender(const double deltaTime) override;

	void OnEvent(Cori::Event& event) override;
};
