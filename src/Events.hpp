#pragma once
#include <Cori.hpp>

namespace Events {
	class PlayerDied final : public Cori::Core::Event {
	public:
		explicit PlayerDied(const Cori::World::Entity& player) : m_Player(player) {}

		[[nodiscard]] Cori::World::Entity GetPlayer() const {
			return m_Player;
		}

		EVENT_CLASS_TYPE(PlayerDied)
		EVENT_CLASS_CATEGORY(Cori::Core::EventCategoryGameplay)

	private:
		Cori::World::Entity m_Player;
	};

	class PlayerEscaped final : public Cori::Core::Event {
	public:
		PlayerEscaped() {}

		EVENT_CLASS_TYPE(PlayerEscaped)
		EVENT_CLASS_CATEGORY(Cori::Core::EventCategoryGameplay)
	};

	class PlayerTookDamage final : public Cori::Core::Event {
	public:
		explicit PlayerTookDamage(const Cori::World::Entity& player, const float damageTaken, const float desiredCameraTrauma = -1.0f) : m_Player(player), m_Damage(damageTaken), m_DesiredCameraTrauma(desiredCameraTrauma) {}

		[[nodiscard]] Cori::World::Entity GetPlayer() const {
			return m_Player;
		}

		[[nodiscard]] float GetDamageAmount() const {
			return m_Damage;
		}

		[[nodiscard]] float GetDesiredCameraTrauma() const {
			return m_DesiredCameraTrauma;
		}

		EVENT_CLASS_TYPE(PlayerTookDamage)
		EVENT_CLASS_CATEGORY(Cori::Core::EventCategoryGameplay)

	private:
		Cori::World::Entity m_Player;
		float m_Damage;
		float m_DesiredCameraTrauma{ -1.0f };
	};

}