#pragma once
#include <Cori.hpp>
#include "Animations.hpp"
#include "Components.hpp"

namespace States {
	namespace Player {
		class Idle : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				//CORI_CORE_INFO("Entity ID {0} entering Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::Idle>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				//CORI_CORE_INFO("Entity ID {0} updating Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::Idle>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
				//CORI_CORE_INFO("Entity ID {0} exiting Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			}

			const char* GetDebugName() const {
				return "IdleState";
			}
		};

		class Run : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				//CORI_CORE_INFO("Entity ID {0} entering RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::Run>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				//CORI_CORE_INFO("Entity ID {0} updating RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::Run>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
				//CORI_CORE_INFO("Entity ID {0} exiting RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			}

			const char* GetDebugName() const {
				return "RunState";
			}
		};

		class Fall : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {

				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::Fall>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::Fall>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "FallState";
			}
		};

		class Jump : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSequence<Animations::Player::JumpStart, Animations::Player::JumpMid>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSequence();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "JumpState";
			}
		};

		class DoubleJump : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::DoubleJump>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::DoubleJump>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "DoubleJumpState";
			}
		};

		class WallJump : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSequence<Animations::Player::JumpStart, Animations::Player::JumpMid>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSequence();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "WallJumpState";
			}
		};

		class WallSlide : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::WallSlide>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::WallSlide>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "WallSlideState";
			}
		};
		// ascending state doesn't always gets triggered when player goes "up", most of the time it is some of the jump states
		// this one exists only to change animation when player was touching the wall but now he is above it and didn't move to one of the sides
		class Ascending : public Cori::State {
		public:
			void OnEnter(Cori::Entity& player, Cori::StateMachine* fsm) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.StartSingle<Animations::Player::JumpMid>();
			}

			void OnUpdate(Cori::Entity& player, Cori::StateMachine* fsm, float deltaTime) override {
				auto& ar = player.GetComponents<Cori::Components::Entity::QuadAnimator>();
				ar.UpdateSingle<Animations::Player::JumpMid>();
			}

			void OnExit(Cori::Entity& player, Cori::StateMachine* fsm) override {
			}

			const char* GetDebugName() const {
				return "AscendingState";
			}
		};
	}

}
