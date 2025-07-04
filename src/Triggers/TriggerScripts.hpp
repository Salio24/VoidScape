#pragma once
#include <Cori.hpp>

class TestTrigger : public Cori::Physics::TriggerBehaviour {
public:
	TestTrigger() = default;
	void OnEnter(Cori::Entity& entity) override {
		CORI_TRACE_TAGGED({ "Trigger" }, "Enter");
	}

	void OnTickUpdate(Cori::Entity& entity, const float timeStep) override {
		//CORI_TRACE_TAGGED({ "Trigger" }, "Update");
	}

	void OnExit(Cori::Entity& entity) override {
		CORI_TRACE_TAGGED({ "Trigger" }, "Exit");
	};

	const char* GetDebugName() const override {
		return "TestTrigger";
	}
};