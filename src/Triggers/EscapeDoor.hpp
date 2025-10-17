#pragma once
#include <Cori.hpp>
#include "Tags.hpp"
#include "Events.hpp"

namespace Triggers {
	class EscapeDoor final : public Cori::World::TriggerBehaviour {
	public:
		EscapeDoor() = default;
		void OnEnter(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
			if (entity.HasComponents<Tags::CharacterTag>()) {
				entity.SetActive(false);
				auto event = Events::PlayerEscaped{};
				Cori::Core::Application::EmitEvent(event);
			}
		}

		void OnTickUpdate(Cori::World::Entity& entity, Cori::World::Entity& trigger, const float timeStep) override {
		}

		void OnExit(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
		}

		const char* GetDebugName() const override {
			return "TestTrigger";
		}
	};
}