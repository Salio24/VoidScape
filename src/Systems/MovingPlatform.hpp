#pragma once
#include <Cori.hpp>

	struct PlatformParams {
		Cori::Math::Function<double, 1> expressionX;
		Cori::Math::Function<double, 1> expressionY;
		Cori::Physics::Vec2 positionOffset;
		std::shared_ptr<Cori::Graphics::SpriteAtlas> atlas;
		uint16_t bodyTileID;
		uint16_t leftOrBottomCornerTileID;
		uint16_t rightOrTopCornerTileID;
		uint16_t size;
		bool vertical;
		uint8_t m_Depth;
	};
namespace Systems {

	class MovingPlatform final : public Cori::World::System {
	public:
		MovingPlatform();
		~MovingPlatform() override;

		MovingPlatform(const MovingPlatform&) = delete;
		MovingPlatform& operator=(const MovingPlatform&) = delete;
		MovingPlatform(MovingPlatform&&) = delete;
		MovingPlatform& operator=(MovingPlatform&&) = delete;

		void OnUpdate(Cori::Core::GameTimer& gameTimer) override;

		void OnTickUpdate(Cori::Core::GameTimer& gameTimer) override;

		void OnImGuiRender(Cori::Core::GameTimer& gameTimer) override;

		void CreatePlatform(PlatformParams& params);

		void Reset(const Cori::Core::GameTimer& gameTimer);

		static std::shared_ptr<MovingPlatform> Create();
		static constexpr Cori::World::SystemPriority Priority = 5;
	private:
		double m_LevelStartStamp{ 0.0f };
	};

}
