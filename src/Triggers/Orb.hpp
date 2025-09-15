#pragma once
#include <Cori.hpp>
#include "Tags.hpp"
#include "Player/Components.hpp"

namespace Triggers {
	class RegularOrb final : public Cori::World::TriggerBehaviour {
	public:
		RegularOrb() = default;
		const char* GetDebugName() const override {
			return "TestTrigger";
		}

		float m_TimeBonus{ 5.0f };

	protected:
		void OnEnter(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
			if (entity.GetComponents<Cori::World::Components::Entity::Tag>().m_Tag == Tags::Character) {
				auto& hc = entity.GetComponents<Components::Health>();
				hc.m_TimeS += m_TimeBonus;
				trigger.SetActive(false);
			}
		}

		void OnTickUpdate(Cori::World::Entity& entity, Cori::World::Entity& trigger, const float timeStep) override {
		}

		void OnExit(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
		}

	};
}