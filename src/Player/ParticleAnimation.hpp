#pragma once
#include <Cori.hpp>
#include "Animations.hpp"

static void TickParticleUpdate(const Cori::Entity& player) {
	auto& fsm = player.GetComponents<Cori::Components::Entity::StateMachine>();

	auto particles = player.FindChildByName("Movement Particles Independent Root");

	if (particles) {
		//CORI_INFO("AA");
		auto landingParticles = particles.value().FindChildByName("Landing Particles");
		auto jumpingParticles = particles.value().FindChildByName("Jumping Particles");
		auto wallJumpParticles = particles.value().FindChildByName("WallJump Particles");
		auto doubleJumpParticles = particles.value().FindChildByName("DoubleJump Particles");

		if (landingParticles) {
			if (landingParticles->IsActiveGlobally()) {
				static bool active = false;
				if (typeid(*fsm.GetCurrentState()) == typeid(States::Player::Idle) || typeid(*fsm.GetCurrentState()) == typeid(States::Player::Jump) || typeid(*fsm.GetCurrentState()) == typeid(States::Player::Run)) {
					active = true;
				}
				if (active) {
					static bool oneShot = true;
					auto& pt = landingParticles->GetComponents<Cori::Components::Entity::Transform>();
					if (oneShot) {
						auto& pa = landingParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
						pa.StartSingle(Animations::Player::Particles::Landing);
						oneShot = false;
					} else {
						pt.SetFrozenState(true);
						auto& pa = landingParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
						bool running = pa.UpdateSingle(Animations::Player::Particles::Landing);
						if (!running) {
							landingParticles->SetActive(false);
							pt.SetFrozenState(false);
							oneShot = true;
							active = false;
						}
					}
				} else {
					landingParticles->SetActive(false);
				}
			}
		} else {
			CORI_ERROR("Failed to retrieve child entity: Landing Particles. Error: {}", landingParticles.error().what());
		}

		if (jumpingParticles) {
			if (jumpingParticles->IsActiveGlobally()) {
				static bool oneShot = true;
				auto& pt = jumpingParticles->GetComponents<Cori::Components::Entity::Transform>();
				if (oneShot) {
					auto& pa = jumpingParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::Jump);
					oneShot = false;
				}
				else {
					pt.SetFrozenState(true);
					auto& pa = jumpingParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					bool running = pa.UpdateSingle(Animations::Player::Particles::Jump);
					if (!running) {
						jumpingParticles->SetActive(false);
						pt.SetFrozenState(false);
						oneShot = true;
					}
				}
			}
		}
		else {
			CORI_ERROR("Failed to retrieve child entity: Jumping Particles. Error: {}", landingParticles.error().what());
		}

		if (wallJumpParticles) {
			if (wallJumpParticles->IsActiveGlobally()) {
				static bool oneShot = true;
				auto& pt = wallJumpParticles->GetComponents<Cori::Components::Entity::Transform>();
				if (oneShot) {
					auto& pa = wallJumpParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::WallJump);
					oneShot = false;
				}
				else {
					pt.SetFrozenState(true);
					auto& pa = wallJumpParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					bool running = pa.UpdateSingle(Animations::Player::Particles::WallJump);
					if (!running) {
						wallJumpParticles->SetActive(false);
						pt.SetFrozenState(false);
						oneShot = true;
					}
				}
			}
		}
		else {
			CORI_ERROR("Failed to retrieve child entity: WallJump Particles. Error: {}", landingParticles.error().what());
		}

		if (doubleJumpParticles) {
			if (doubleJumpParticles->IsActiveGlobally()) {
				static bool oneShot = true;
				auto& pt = doubleJumpParticles->GetComponents<Cori::Components::Entity::Transform>();
				if (oneShot) {
					auto& pa = doubleJumpParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					pa.StartSingle(Animations::Player::Particles::DoubleJump);
					oneShot = false;
				}
				else {
					pt.SetFrozenState(true);
					auto& pa = doubleJumpParticles->GetComponents<Cori::Components::Entity::QuadAnimator>();
					bool running = pa.UpdateSingle(Animations::Player::Particles::DoubleJump);
					if (!running) {
						doubleJumpParticles->SetActive(false);
						pt.SetFrozenState(false);
						oneShot = true;
					}
				}
			}
		}
		else {
			CORI_ERROR("Failed to retrieve child entity: WallJump Particles. Error: {}", landingParticles.error().what());
		}
	}
	else {
		CORI_ERROR("Failed to retrieve child entity: Movement Particles Independent Root. Error: {}", particles.error().what());
	}






}