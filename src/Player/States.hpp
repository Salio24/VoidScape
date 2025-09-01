#pragma once
#include <Cori.hpp>
#include "Animations.hpp"
#include "AnimationPacks.hpp"

//#define OLD_TEST

namespace States {
	namespace Player {
		class Fall final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Fall), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");
				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto landingParticles = particles->FindChildByName("Landing Particles");
					if (landingParticles) {
						landingParticles->SetActive(true);
					} else {
						CORI_ERROR("Failed to retrieve child entity: Landing Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			const char* GetDebugName() const override {
				return "FallState";
			}
		};

		class Run final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Run), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1.value().SetActive(true);
					auto& pa = particles1.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::RunFrontO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					particles2.value().SetActive(true);
					auto& pa = particles2.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::RunBackO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					auto& pa = particles1.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::RunFrontO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					auto& pa = particles2.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::RunBackO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1.value().SetActive(false);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					particles2.value().SetActive(false);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			const char* GetDebugName() const override {
				return "RunState";
			}
		};

		class Idle final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Idle), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "IdleState";
			}
		};

		class Jump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto jumpStart = std::make_pair(pack->GetAnimation(Animations::Player::JumpStart), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				const auto jumpMid = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(jumpStart, jumpMid);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");

				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto jumpingParticles = particles->FindChildByName("Jumping Particles");
					if (jumpingParticles) {
						jumpingParticles->SetActive(true);
					} else {
						CORI_ERROR("Failed to retrieve child entity: Jumping Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "JumpState";
			}
		};

		class DoubleJump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::DoubleJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");
				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto doubleJumpParticles = particles->FindChildByName("DoubleJump Particles");
					if (doubleJumpParticles) {
						doubleJumpParticles->SetActive(true);
					} else {
						CORI_ERROR("Failed to retrieve child entity: DoubleJump Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "DoubleJumpState";
			}
		};

		class WallJump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto independentParticlesRoot = player.FindChildByName("Movement Particles Independent Root");
				auto particles = player.FindChildByName("Movement Particles Independent Root");
				if (particles) {
					auto wallJumpParticles = particles->FindChildByName("WallJump Particles");
					if (wallJumpParticles) {
						wallJumpParticles->SetActive(true);
					} else {
						CORI_ERROR("Failed to retrieve child entity: WallJump Particles. Error: {}", particles.error().what());
					}
				} else {
					CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "WallJumpState";
			}
		};

		class WallSlide final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					particles.value().SetActive(true);
					auto& pa = particles.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::WallSlideO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					auto& pa = particles.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::WallSlideO);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);

				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1.value().SetActive(false);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}
			}

			const char* GetDebugName() const override {
				return "WallSlideState";
			}
		};
		// ascending state doesn't always gets triggered when player goes "up", most of the time it is some of the jump states
		// this one exists only to change animation when player was touching the wall but now he is above it and didn't move to one of the sides
		class Ascending final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player) override {
				const auto pack = Cori::AssetManager::GetAnimationPack(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::Entity& player) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimatorNew>();
				ar.Stop(true);
			}

			const char* GetDebugName() const override {
				return "AscendingState";
			}
		};
	}

}
