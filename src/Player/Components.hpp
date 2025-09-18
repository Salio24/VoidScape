#pragma once
#include <Cori.hpp>
#include "States.hpp"
#include "Events.hpp"

namespace Components {
	struct Health {
		Health() = default;
		Health(const float initialValue, const Cori::World::Entity& entity) : m_TimeS(initialValue), m_InitialValue(initialValue), m_Player(entity) {}

		Health(const Health&) = delete;
		Health& operator=(const Health&) = delete;

		void OnTickUpdate(const float timeStep, const bool levelComplete) {
			auto& fsm = m_Player.GetComponents<Cori::World::Components::Entity::StateMachine>();

			if (!fsm.IsInState<States::Player::Dead>()) {
				if (m_TimeS > 0.0f && !levelComplete) {
					m_TimeS -= timeStep;
				} else if (!levelComplete) {
					m_LastTime = m_TimeS;
					auto event = Events::PlayerDied(m_Player);
					Cori::Core::Application::EmitEvent(event);
					fsm.SetState<States::Player::Dead>();
					return;
				}

				m_LastTime = m_TimeS;
			}
		}

		void Reset() {
			m_TimeS = m_InitialValue;
		}

		double m_TimeS{ 0.0 };
		double m_LastTime{ 0.0 };
	private:
		float m_InitialValue{ 0.0f };
		Cori::World::Entity m_Player;
	};

	struct Spawnpoint {
		Spawnpoint() = default;
		explicit Spawnpoint(const glm::vec2& point)
			: m_Spawnpoint(point) {}
		glm::vec2 m_Spawnpoint{ 0.0f, 0.0f };
	};
}