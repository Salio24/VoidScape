#pragma once
#include <Cori.hpp>

class GameLayer : public Cori::Layer {
public:
	GameLayer();

	~GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(const double deltaTime, const double tickAlpha) override;
	virtual void OnTickUpdate(const float timeStep) override;
	virtual void OnImGuiRender(const double deltaTime) override;

	virtual void OnEvent(Cori::Event& event) override;
};
