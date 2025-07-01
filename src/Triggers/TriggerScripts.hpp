#pragma once
#include <Cori.hpp>

class TestTrigger : public Cori::Physics::TriggerBehaviour {
public:
	TestTrigger() = default;
	void OnEnter(Cori::Entity& entity) override {
	}

	void OnTickUpdate(Cori::Entity& entity, const float timeStep) override {
	}

	void OnExit(Cori::Entity& entity) override {
	};

	const char* GetDebugName() const override {
		return "TestTrigger";
	}
};