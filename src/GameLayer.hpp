#pragma once
#include <Cori.hpp>

class GameLayer : public Cori::Layer {
public:
	GameLayer();

	~GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(const double deltaTime) override;
	virtual void OnTickUpdate() override;
	virtual void OnImGuiRender(const double deltaTime);

	virtual void OnEvent(Cori::Event& event);

};
