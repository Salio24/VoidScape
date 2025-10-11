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
		void OnUpdate(Cori::Core::GameTimer& gameTimer) override;

		void OnTickUpdate(Cori::Core::GameTimer& gameTimer) override;

		void OnImGuiRender(Cori::Core::GameTimer& gameTimer) override;

		void CreatePlatform(PlatformParams& params);

		void Reset();

		bool Create();
		static constexpr Cori::World::SystemPriority Priority = 1000;
	private:
		double m_InternalTimer{ 0.0f };
	};

}
