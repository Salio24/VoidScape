#include "MovingPlatform.hpp"
#include "Tags.hpp"
#include "Components.hpp"

namespace Systems {
	void MovingPlatform::OnUpdate(Cori::Core::GameTimer& gameTimer) {
		auto view = m_Owner.View<Components::MovingPlatform, Cori::World::Components::Entity::Transform>(Cori::World::Exclude<Cori::World::Components::Entity::InactiveLocallyFlag>());
		for (const auto entity : view) {
			auto& pc = view.Get<Components::MovingPlatform>(entity);
			auto& tr = view.Get<Cori::World::Components::Entity::Transform>(entity);
			tr.SetLocalPosition(Cori::Physics::ToPixels(pc.m_CurrentPosition * gameTimer.GetTickAlpha() + pc.m_OldPosition * (1.0f - gameTimer.GetTickAlpha())));
		}


	}

	void MovingPlatform::OnTickUpdate(Cori::Core::GameTimer& gameTimer) {

	}

	void MovingPlatform::OnImGuiRender(Cori::Core::GameTimer& gameTimer) {
		const double elapsedLevelS = gameTimer.GetElapsedSeconds() - m_LevelStartStamp;

		auto view = m_Owner.View<Components::MovingPlatform, Cori::World::Components::Entity::RigidBody>(Cori::World::Exclude<Cori::World::Components::Entity::InactiveLocallyFlag>());

		for (const auto entity : view) {
			auto& rb = view.Get<Cori::World::Components::Entity::RigidBody>(entity);
			auto& pc = view.Get<Components::MovingPlatform>(entity);

			pc.m_OldPosition = pc.m_CurrentPosition;
			Cori::Physics::Transform transform;
			transform.p = {
				static_cast<float>(pc.m_ExpressionX(elapsedLevelS)) + pc.m_PositionOffset.x,
				static_cast<float>(pc.m_ExpressionY(elapsedLevelS)) + pc.m_PositionOffset.y
			};
			pc.m_CurrentPosition = transform.p;
			transform.q = b2Rot_identity;

			rb.SetTargetTransform(transform, gameTimer.GetTimestep());
		}

	}

	void MovingPlatform::CreatePlatform(PlatformParams& params) {
		static uint32_t count = 0;
		Cori::World::Entity platform = m_Owner.CreateEntity(std::format("Moving Platform {}", count), Tags::MovingPlatform);
		++count;
		auto& tr = platform.GetComponents<Cori::World::Components::Entity::Transform>();
		tr.SetLocalDepth(params.m_Depth);

		auto& pc = platform.AddComponent<Components::MovingPlatform>();
		pc.m_ExpressionX = std::move(params.expressionX);
		pc.m_ExpressionY = std::move(params.expressionY);
		pc.m_PositionOffset = params.positionOffset;

		Cori::Physics::Body::Params bp;
		bp.type = b2_kinematicBody;
		bp.name = "Platform";
		bp.position = pc.m_PositionOffset;
		bp.fixedRotation = true;
		bp.rotation = b2Rot_identity;

		auto& rb = platform.AddComponent<Cori::World::Components::Entity::RigidBody>(m_Owner.GetContextComponent<Cori::World::Components::Scene::PhysicsWorld>(), bp, platform);

		// false when even
		bool oddPlacement = false;
		Cori::Physics::Vec2 halfSize;
		if (params.size % 2) {
			oddPlacement = true;
		}

		if (params.vertical) {
			halfSize = { 0.5f, static_cast<float>(params.size) / 2.0f };
		} else {
			halfSize = { static_cast<float>(params.size) / 2.0f, 0.5f };
		}

		Cori::Physics::Shape::Params sp;
		rb.CreateShape(Cori::Physics::DestroyWithParent, sp, Cori::Physics::Polygon::CreateBox(halfSize));

		uint16_t partCount = 0;

		#if 1
		uint16_t size = 0;
		float bonusX = 0.0f;

		if (!oddPlacement) {
			size = params.size / 2;
			bonusX = CORI_PIXELS_PER_METER / 2.0f;
		} else {
			size = params.size / 2 + 1;
		}

		for (uint16_t i = 0; i < size; ++i) {
			glm::vec2 pos;
			if (!params.vertical) {
				pos = glm::vec2(CORI_PIXELS_PER_METER * i + bonusX, 0.0f);
			} else {
				pos = glm::vec2(0.0f, CORI_PIXELS_PER_METER * i + bonusX);
			}

			Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
			++partCount;
			auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
			transform.SetLocalPosition(pos);
			if (i != size - 1) {
				part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
			} else {
				part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.rightOrTopCornerTileID));
			}
			CORI_CHECK_EXPECTED(part.SetParent(platform));
		}

		for (uint16_t i = 0; i < size; ++i) {
			glm::vec2 pos;
			if (!params.vertical) {
				pos = glm::vec2(-(CORI_PIXELS_PER_METER * i + bonusX), 0.0f);
			} else {
				pos = glm::vec2(0.0f, -(CORI_PIXELS_PER_METER * i + bonusX));
			}

			Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
			++partCount;
			auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
			transform.SetLocalPosition(pos);
			if (i != size - 1) {
				part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
			} else {
				part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.leftOrBottomCornerTileID));
			}
			CORI_CHECK_EXPECTED(part.SetParent(platform));
		}

		#else
		if (!oddPlacement) {
			const uint16_t size = params.size.x / 2;

			for (uint16_t i = 0; i < size; ++i) {
				glm::vec2 pos = glm::vec2(CORI_PIXELS_PER_METER * i + CORI_PIXELS_PER_METER / 2.0f, 0.0f);
				Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
				++partCount;
				auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
				transform.SetLocalPosition(pos);
				if (i != size - 1) {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
				} else {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.rightCornerTileID));
				}
				CORI_CHECK_EXPECTED(part.SetParent(platform));
			}

			for (uint16_t i = 0; i < size; ++i) {
				glm::vec2 pos = glm::vec2(-(CORI_PIXELS_PER_METER * i + CORI_PIXELS_PER_METER / 2.0f), 0.0f);
				Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
				++partCount;
				auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
				transform.SetLocalPosition(pos);
				if (i != size - 1) {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
				} else {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.leftCornerTileID));
				}
				CORI_CHECK_EXPECTED(part.SetParent(platform));
			}
		} else {
			const uint16_t size = params.size.x / 2 + 1;

			for (uint16_t i = 0; i < size; ++i) {
				glm::vec2 pos = glm::vec2(CORI_PIXELS_PER_METER * i, 0.0f);
				Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
				++partCount;
				auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
				transform.SetLocalPosition(pos);
				if (i != size - 1) {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
				} else {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.rightCornerTileID));
				}
				CORI_CHECK_EXPECTED(part.SetParent(platform));
			}

			for (uint16_t i = 0; i < size; ++i) {
				glm::vec2 pos = glm::vec2(-(CORI_PIXELS_PER_METER * i), 0.0f);
				Cori::World::Entity part = m_Owner.CreateEntity(std::format("Platforms' '{}' part '{}'", count, partCount), Tags::MovingPlatform);
				++partCount;
				auto& transform = part.GetComponents<Cori::World::Components::Entity::Transform>();
				transform.SetLocalPosition(pos);
				if (i != size - 1) {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.bodyTileID));
				} else {
					part.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{CORI_PIXELS_PER_METER / 2.0f, CORI_PIXELS_PER_METER / 2.0f}, params.atlas->GetTexture(), params.atlas->GetSpriteUVsAtIndex(params.leftCornerTileID));
				}
				CORI_CHECK_EXPECTED(part.SetParent(platform));
			}
		}
		#endif


	}

	void MovingPlatform::Reset(const Cori::Core::GameTimer& gameTimer) {
		m_LevelStartStamp = gameTimer.GetElapsedSeconds();
	}

	bool MovingPlatform::Create() {
		return true;
	}
}
