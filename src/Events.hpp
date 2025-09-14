#pragma once
#include <Cori.hpp>

namespace Events {
	class PlayerDied final : public Cori::Core::Event {
	public:
		explicit PlayerDied(const Cori::World::Entity& player) : m_Player(player) {}

		[[nodiscard]] Cori::World::Entity GetPlayer() const {
			return m_Player;
		}

		EVENT_CLASS_TYPE(GameUserDefinedEvent)
		EVENT_CLASS_CATEGORY(Cori::Core::EventCategoryGameplay)

	private:
		Cori::World::Entity m_Player;
	};

	class PlayerEscaped final : public Cori::Core::Event {
	public:
		PlayerEscaped() {}

		EVENT_CLASS_TYPE(GameUserDefinedEvent)
		EVENT_CLASS_CATEGORY(Cori::Core::EventCategoryGameplay)
	};

}