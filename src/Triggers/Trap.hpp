#pragma once
#include <Cori.hpp>
#include "Tags.hpp"
#include "../Components.hpp"

namespace Triggers {
	class SpikeTrap final : public Cori::World::TriggerBehaviour {
	public:
		SpikeTrap() = default;
		const char* GetDebugName() const override {
			return "TestTrigger";
		}

		float m_Damage{ 0.0f };
		uint32_t m_InvisibilityTicksBonus{ 0 };
		uint32_t m_HitStunTicksPenalty{ 0 };
		Cori::Physics::Vec2 m_HitImpulse{ 0.0f, 0.0f };
		Cori::Physics::Vec2 m_HitPlayerVelocityModifier{ 1.0f, 1.0f };

	protected:
		void OnEnter(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
			if (entity.HasComponents<Tags::CharacterTag>()) {
				auto& hc = entity.GetComponents<Components::Health>();
				const bool damageDelt = hc.InduceDamage(m_Damage);
				if (damageDelt) {
					auto& mover = entity.GetComponents<Components::Mover>();
					mover.AddImpulse(m_HitImpulse, m_HitPlayerVelocityModifier);
					mover.AddStunTicks(m_HitStunTicksPenalty);
					hc.AddInvisibilityTicks(m_InvisibilityTicksBonus);
					auto event = Events::PlayerTookDamage(entity, m_Damage);
					Cori::Core::Application::EmitEvent(event);
				}
			}
		}

		void OnTickUpdate(Cori::World::Entity& entity, Cori::World::Entity& trigger, const float timeStep) override {
			if (entity.HasComponents<Tags::CharacterTag>()) {
				auto& hc = entity.GetComponents<Components::Health>();
				const bool damageDelt = hc.InduceDamage(m_Damage);
				if (damageDelt) {
					auto& mover = entity.GetComponents<Components::Mover>();
					mover.AddImpulse(m_HitImpulse, m_HitPlayerVelocityModifier);
					mover.AddStunTicks(m_HitStunTicksPenalty);
					hc.AddInvisibilityTicks(m_InvisibilityTicksBonus);
					auto event = Events::PlayerTookDamage(entity, m_Damage);
					Cori::Core::Application::EmitEvent(event);
				}
			}
		}

		void OnExit(Cori::World::Entity& entity, Cori::World::Entity& trigger) override {
		}

	};
}