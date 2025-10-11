#pragma once
#include <Cori.hpp>
#include "Animations.hpp"
#include "AnimationPacks.hpp"

namespace States {
	namespace Player {
		class Run final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Run), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunFront), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp1 = *tmp;
							temp1.SetParent(player);
						}
					}
				}

				auto result2 = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result2) {
					auto system = result2->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunBack), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp2 = *tmp;
							temp2.SetParent(player);
						}
					}
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);

				temp1.UnlinkFromParent();
				temp2.UnlinkFromParent();
				temp1.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(false);
				temp2.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(false);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "RunState";
			}

			Cori::World::Entity temp1;
			Cori::World::Entity temp2;
		};

		class Idle final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Idle), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "IdleState";
			}
		};

		class Jump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto jumpStart = std::make_pair(pack->GetAnimation(Animations::Player::JumpStart), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				const auto jumpMid = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(jumpStart, jumpMid);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Jump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
					}
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "JumpState";
			}
		};

		class Fall final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Fall), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (nextStateType == typeid(Idle) || nextStateType == typeid(Jump) || nextStateType == typeid(Run)) {
					auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
					if (result) {
						auto system = result->GetSystem<Cori::World::Systems::Animation>();
						if (system) {
							auto locked = system->lock();
							const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
							const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Landing), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
							auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
							locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
						}
					}
				}


			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "FallState";
			}
		};

		class DoubleJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::DoubleJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::DoubleJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
					}
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "DoubleJumpState";
			}
		};

		class WallJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						glm::vec2 scale = tr.GetLocalScale();
						glm::vec2 pos = tr.GetLocalPosition();
						locked->PlayAnimation({ pos.x - FXanim.first.GetFrameSize().x / 16.0f * Cori::Math::Sign(scale.x), pos.y }, tr.GetLocalDepthOffset(), { -scale.x, scale.y }, tr.GetLocalRotation(), FXanim);
					}
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "WallJumpState";
			}
		};

		class WallSlide final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp = *tmp;
							temp.SetParent(player);
						}
					}
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);

				temp.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(true);
				temp.UnlinkFromParent();
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "WallSlideState";
			}

			Cori::World::Entity temp;
		};
		// ascending state doesn't always gets triggered when player goes "up", most of the time it is some of the jump states
		// this one exists only to change animation when player was touching the wall but now he is above it and didn't move to one of the sides
		class Ascending final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				ar.Stop(true);
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "AscendingState";
			}
		};

		class Dead final : public Cori::World::EntityState {
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Dead), Cori::Graphics::Animation::PlayParams{});
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "Dead";
			}
		};
	}

}
