#pragma once
#include <Cori.hpp>
#include "Animations.hpp"
#include "AnimationPacks.hpp"

namespace States {
	namespace Player {
		class Run final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Run), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1->SetActive(true);

					const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
					auto& qa = particles1->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunFront), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
					qa.Play(FXanim);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					auto& t = particles2->GetComponents<Cori::World::Components::Entity::Transform>();
					t.SetDetachedState(false);

					particles2->SetActive(true);

					//const auto FXpack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovementFX);
					const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
					auto& qa = particles2->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunBack), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
					qa.Play(FXanim);

					qa.SetStopCallback([particles2] mutable {
						if (particles2->IsValid()) {
							particles2->SetActive(false);
						}
					});
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1->SetActive(false);
					auto& qa = particles1->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					qa.Stop(true);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {

					auto& t = particles2->GetComponents<Cori::World::Components::Entity::Transform>();
					auto& qa = particles2->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					if (qa.GetTicksElapsed() > 1) {
						t.SetDetachedState(true);
						qa.Stop(false);
					} else {
						qa.Stop(true);
						particles2->SetActive(false);
					}
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			const char* GetDebugName() const override {
				return "RunState";
			}
		};

		class Idle final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Idle), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "IdleState";
			}
		};

		class Jump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto jumpStart = std::make_pair(pack->GetAnimation(Animations::Player::JumpStart), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				const auto jumpMid = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(jumpStart, jumpMid);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");

				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto jumpingParticles = particles->FindChildByName("Jumping Particles");
					if (jumpingParticles) {
						jumpingParticles->SetActive(true);

						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						auto& qa = jumpingParticles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
						const auto anim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Jump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						qa.Play(anim);

						qa.SetStopCallback([jumpingParticles] mutable {
							if (jumpingParticles->IsValid()) {
								jumpingParticles->SetActive(false);
								auto& t_ = jumpingParticles->GetComponents<Cori::World::Components::Entity::Transform>();
								t_.SetDetachedState(false);
							}
						});
						qa.SetNextTickCallback([jumpingParticles] mutable {
							if (jumpingParticles->IsValid()) {
								auto& t = jumpingParticles->GetComponents<Cori::World::Components::Entity::Transform>();
								t.SetDetachedState(true);
							}
						});
					}
					else {
						CORI_ERROR("Failed to retrieve child entity: Jumping Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "JumpState";
			}
		};

		class Fall final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Fall), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				if (nextStateType == typeid(Idle) || nextStateType == typeid(Jump) || nextStateType == typeid(Run)) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					ar.Stop(true);

					auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");
					auto particles = player.FindChildByName("Movement Particles Independent Root");
					if (particles) {
						auto landingParticles = particles->FindChildByName("Landing Particles");
						if (landingParticles) {
							landingParticles->SetActive(true);

							const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
							auto& qa = landingParticles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
							const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Landing), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
							qa.Play(FXanim);

							qa.SetStopCallback([landingParticles] mutable {
								if (landingParticles->IsValid()) {
									landingParticles->SetActive(false);
									auto& t = landingParticles->GetComponents<Cori::World::Components::Entity::Transform>();
									t.SetDetachedState(false);
								}
							});
							qa.SetNextTickCallback([landingParticles] mutable {
								if (landingParticles->IsValid()) {
									auto& t = landingParticles->GetComponents<Cori::World::Components::Entity::Transform>();
									t.SetDetachedState(true);
								}
							});
						}
						else {
							CORI_ERROR("Failed to retrieve child entity: Landing Particles. Error: {}", particles.error().what());
						}
					}
					else {
						CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
					}
				}
			}

			const char* GetDebugName() const override {
				return "FallState";
			}
		};

		class DoubleJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::DoubleJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");
				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto doubleJumpParticles = particles->FindChildByName("DoubleJump Particles");
					if (doubleJumpParticles) {
						auto& t = doubleJumpParticles->GetComponents<Cori::World::Components::Entity::Transform>();
						t.SetDetachedState(true);
						doubleJumpParticles->SetActive(true);

						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						auto& qa = doubleJumpParticles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::DoubleJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						qa.Play(FXanim);

						qa.SetStopCallback([doubleJumpParticles] mutable {
							if (doubleJumpParticles->IsValid()) {
								doubleJumpParticles->SetActive(false);
								auto& t_ = doubleJumpParticles->GetComponents<Cori::World::Components::Entity::Transform>();
								t_.SetDetachedState(false);
							}
						});
					} else {
						CORI_ERROR("Failed to retrieve child entity: DoubleJump Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "DoubleJumpState";
			}
		};

		class WallJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto wallJumpParticles = particles->FindChildByName("WallJump Particles");
					if (wallJumpParticles) {
						wallJumpParticles->SetActive(true);

						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						auto& qa = wallJumpParticles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						qa.Play(FXanim);
						auto& qr = wallJumpParticles->GetComponents<Cori::World::Components::Entity::QuadRenderer>();
						auto& t = wallJumpParticles->GetComponents<Cori::World::Components::Entity::Transform>();
						t.SetLocalPosition({-qr.GetHalfSize().x / 8.0f, 0.0f});
						t.SetLocalScale({-1.0f, 1.0f});
						t.SetDetachedState(true);

						qa.SetStopCallback([wallJumpParticles] mutable {
							if (wallJumpParticles->IsValid()) {
								wallJumpParticles->SetActive(false);
								auto& t_ = wallJumpParticles->GetComponents<Cori::World::Components::Entity::Transform>();
								t_.SetLocalPosition({0.0f, 0.0f});
								t_.SetLocalScale({1.0f, 1.0f});
								t_.SetDetachedState(false);
							}
						});
					} else {
						CORI_ERROR("Failed to retrieve child entity: WallJump Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "WallJumpState";
			}
		};

		class WallSlide final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					particles->SetActive(true);

					const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
					auto& qa = particles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
					qa.Play(FXanim);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);

				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					auto& qa = particles->GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
					qa.Stop(true);
					particles->SetActive(false);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			const char* GetDebugName() const override {
				return "WallSlideState";
			}
		};
		// ascending state doesn't always gets triggered when player goes "up", most of the time it is some of the jump states
		// this one exists only to change animation when player was touching the wall but now he is above it and didn't move to one of the sides
		class Ascending final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "AscendingState";
			}
		};

		class Dead final : public Cori::World::EntityState {
			void OnEnter(Cori::World::Entity& player) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Dead), Cori::Graphics::Animation::PlayParams{});
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_info& nextStateType) override {
			}

			const char* GetDebugName() const override {
				return "Dead";
			}
		};
	}

}
