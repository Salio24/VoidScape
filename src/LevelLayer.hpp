#pragma once
#include <Cori.hpp>
#include "Mover.hpp"


class LevelLayer : public Cori::Layer {
public:
	LevelLayer();

	~LevelLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(const double deltaTime) override;
	virtual void OnTickUpdate(const float timeStep) override;
	virtual void OnImGuiRender(const double deltaTime) override;

	virtual void OnEvent(Cori::Event& event) override;

private:
	bool m_PhysicsDebugDraw{ true };

	Mover mover;
};
