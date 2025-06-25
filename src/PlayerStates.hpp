#pragma once
#include <Cori.hpp>
#include "PlayerAnimations.hpp"

namespace PlayerStates {
	class IdleState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			//CORI_CORE_INFO("Entity ID {0} entering Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSingle<Animations::PlayerIdle>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			//CORI_CORE_INFO("Entity ID {0} updating Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSingle < Animations::PlayerIdle>();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			//CORI_CORE_INFO("Entity ID {0} exiting Idle. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
		}

		const char* GetDebugName() const {
			return "IdleState";
		}
	};


	class RunState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			//CORI_CORE_INFO("Entity ID {0} entering RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSingle<Animations::PlayerRun>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			//CORI_CORE_INFO("Entity ID {0} updating RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSingle<Animations::PlayerRun>();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			//CORI_CORE_INFO("Entity ID {0} exiting RunState. {1}", static_cast<uint32_t>(player), Cori::Application::GetTest());
		}

		const char* GetDebugName() const {
			return "RunState";
		}
	};

	class FallState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {

			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSingle<Animations::PlayerFall>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSingle<Animations::PlayerFall>();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {

		}

		const char* GetDebugName() const {
			return "FallState"; 
		}
	};

	class JumpState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSequence<Animations::PlayerJumpStart, Animations::PlayerJumpMid>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSequence();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
		}

		const char* GetDebugName() const {
			return "JumpState";
		}
	};

	class DoubleJumpState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSingle<Animations::PlayerDoubleJump>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSingle<Animations::PlayerDoubleJump>();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
		}

		const char* GetDebugName() const {
			return "DoubleJumpState";
		}
	};

	class WallJumpState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSequence<Animations::PlayerJumpStart, Animations::PlayerJumpMid>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSequence();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
		}

		const char* GetDebugName() const {
			return "WallJumpState";
		}
	};

	class WallSlideState : public Cori::FSM::State {
	public:
		void OnEnter(Cori::Entity player, Cori::FSM::Machine* fsm) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.StartSingle<Animations::PlayerWallSlide>();
		}

		void OnUpdate(Cori::Entity player, Cori::FSM::Machine* fsm, float deltaTime) override {
			auto& ar = player.GetComponents<Cori::Animator>();
			ar.UpdateSingle<Animations::PlayerWallSlide>();
		}

		void OnExit(Cori::Entity player, Cori::FSM::Machine* fsm) override {
		}

		const char* GetDebugName() const {
			return "WallSlideState";
		}
	};


}
