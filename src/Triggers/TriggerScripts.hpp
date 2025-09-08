#pragma once
#include <Cori.hpp>

class TestTrigger : public Cori::World::TriggerBehaviour {
public:
	TestTrigger() = default;
	void OnEnter(Cori::World::Entity& entity) override {

		CORI_TRACE_TAGGED({ "Trigger" }, "Enter by: {}", entity.GetName());
	}

	void OnTickUpdate(Cori::World::Entity& entity, const float timeStep) override {
		//CORI_TRACE_TAGGED({ "Trigger" }, "Update");
	}

	void OnExit(Cori::World::Entity& entity) override {
		CORI_TRACE_TAGGED({ "Trigger" }, "Exit");
	};

	const char* GetDebugName() const override {
		return "TestTrigger";
	}
};