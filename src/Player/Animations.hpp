#pragma once
#include <Cori.hpp>

namespace AnimatorNames {

	inline constexpr char PlayerParticleAnimator[] = "Player Particle Animator";

}

namespace Animations {
	namespace Player {
		inline constexpr uint32_t Idle = 0;
		inline constexpr uint32_t IdleTransition = 17;
		inline constexpr uint32_t DoubleJump = 9;
		inline constexpr uint32_t Run = 2;
		inline constexpr uint32_t WallSlide = 22;
		inline constexpr uint32_t WallJump = 23;
		inline constexpr uint32_t Fall = 7;
		inline constexpr uint32_t JumpStart = 5;
		inline constexpr uint32_t JumpMid = 6;

		namespace Particles {
			inline constexpr Cori::AnimationDescriptor RunFrontO(3, true, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor RunBackO(4, true, AnimatorNames::PlayerParticleAnimator);

			inline constexpr Cori::AnimationDescriptor JumpO(7, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor LandingO(8, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor WallJumpO(14, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor WallSlideO(15, true, AnimatorNames::PlayerParticleAnimator);

			inline constexpr Cori::AnimationDescriptor DoubleJumpO(10, false, AnimatorNames::PlayerParticleAnimator);

			inline constexpr uint32_t RunFront = 3;
			inline constexpr uint32_t RunBack = 4;
			inline constexpr uint32_t Jump = 7;
			inline constexpr uint32_t Landing = 8;
			inline constexpr uint32_t WallJump = 14;
			inline constexpr uint32_t WallSlide = 15;
			inline constexpr uint32_t DoubleJump = 10;


		}
	}
}