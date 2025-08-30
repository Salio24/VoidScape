#pragma once
#include <Cori.hpp>
#include "Animations.hpp"

namespace States {
	namespace Player {
		class Fall final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::Fall);

			}

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::Fall);
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
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
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::Run);
				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					particles1.value().SetActive(true);
					auto& pa = particles1.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::RunFront);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					particles2.value().SetActive(true);
					auto& pa = particles2.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::RunBack);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::Run);
				auto particles1 = player.FindChildByName("Movement Particles Part 1");
				if (particles1) {
					auto& pa = particles1.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::RunFront);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles1.error().what());
				}

				auto particles2 = player.FindChildByName("Movement Particles Part 2");
				if (particles2) {
					auto& pa = particles2.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::RunBack);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 2. Error: {}", particles2.error().what());
				}
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
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
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::Idle);
			}

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::Idle);
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
			}

			const char* GetDebugName() const override {
				return "IdleState";
			}
		};

		class Jump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSequence(Animations::Player::JumpStart, Animations::Player::JumpMid);
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

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSequence();
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
			}

			const char* GetDebugName() const override {
				return "JumpState";
			}
		};

		class DoubleJump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::DoubleJump);
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

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::DoubleJump);
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
			}

			const char* GetDebugName() const override {
				return "DoubleJumpState";
			}
		};

		class WallJump final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::WallJump);
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

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::WallJump);
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
			}

			const char* GetDebugName() const override {
				return "WallJumpState";
			}
		};

		class WallSlide final : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::WallSlide);
				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					particles.value().SetActive(true);
					auto& pa = particles.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::WallSlide);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::WallSlide);
				auto particles = player.FindChildByName("Movement Particles Part 1");
				if (particles) {
					auto& pa = particles.value().GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.UpdateSingle(Animations::Player::Particles::WallSlide);
				} else {
					CORI_ERROR("Failed to retrieve sub entity: Movement Particles Part 1. Error: {}", particles.error().what());
				}
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
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
			void OnEnter(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle(Animations::Player::JumpMid);
			}

			void OnTickUpdate(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle(Animations::Player::JumpMid);
			}

			void OnExit(Cori::Entity& player, Cori::Components::Entity::StateMachine* fsm) override {
			}

			const char* GetDebugName() const override {
				return "AscendingState";
			}
		};
	}

}
