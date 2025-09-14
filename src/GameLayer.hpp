#pragma once
#include <Cori.hpp>

class GameLayer final : public Cori::Core::Layer {
public:
	GameLayer();

	~GameLayer() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Cori::Core::GameTimer& gameTimer) override;
	void OnTickUpdate(Cori::Core::GameTimer& gameTimer) override;
	void OnImGuiRender(Cori::Core::GameTimer& gameTimer) override;

	void OnEvent(Cori::Core::Event& event) override;
};
