#pragma once
#include <Cori.hpp>
#include "Mover.hpp"


class LevelLayer : public Cori::Layer {
public:
	LevelLayer();

	~LevelLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(const double deltaTime, const double tickAlpha) override;
	virtual void OnTickUpdate(const float timeStep) override;
	virtual void OnImGuiRender(const double deltaTime) override;

	virtual void OnEvent(Cori::Event& event) override;

private:
	bool m_PhysicsDebugDraw{ true };
	bool m_MoverDebugDraw{ true };

	Cori::Entity m_Player;

	std::unique_ptr<Mover> m_Mover;
};
