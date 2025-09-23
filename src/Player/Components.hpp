#pragma once
#include <Cori.hpp>
#include "States.hpp"
#include "Events.hpp"

namespace Components {
	struct Health {
		Health() = default;
		Health(const float initialValue, const Cori::World::Entity& entity) : m_TimeHealth(initialValue), m_InitialValue(initialValue), m_Player(entity) {}

		Health(const Health&) = delete;
		Health& operator=(const Health&) = delete;

		void OnTickUpdate(const float timeStep, const bool levelComplete) {
			auto& fsm = m_Player.GetComponents<Cori::World::Components::Entity::StateMachine>();

			if (!fsm.IsInState<States::Player::Dead>()) {
				if (m_TimeHealth > 0.0f && !levelComplete) {
					m_TimeHealth -= timeStep;
				} else if (!levelComplete) {
					m_LastTime = m_TimeHealth;
					auto event = Events::PlayerDied(m_Player);
					Cori::Core::Application::EmitEvent(event);
					fsm.SetState<States::Player::Dead>();
					return;
				}

				m_LastTime = m_TimeHealth;

				if (m_InvisibilityTicksLeft > 0) {
					--m_InvisibilityTicksLeft;
				}
			}
		}

		bool InduceDamage(const float damage) {
			if (m_InvisibilityTicksLeft == 0) {
				m_TimeHealth -= damage;
				return true;
			}
			return false;
		}

		void AddInvisibilityTicks(const uint32_t ticks) {
			m_InvisibilityTicksLeft += ticks;
		}

		void Reset() {
			m_TimeHealth = m_InitialValue;
			m_InvisibilityTicksLeft = 0;
		}

		double m_TimeHealth{ 0.0 };
		double m_LastTime{ 0.0 };
	private:
		float m_InitialValue{ 0.0f };
		uint32_t m_InvisibilityTicksLeft{ 0 };
		Cori::World::Entity m_Player;
	};

	struct Spawnpoint {
		Spawnpoint() = default;
		explicit Spawnpoint(const glm::vec2& point)
			: m_Spawnpoint(point) {}
		glm::vec2 m_Spawnpoint{ 0.0f, 0.0f };
	};
}