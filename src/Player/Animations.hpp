#pragma once
#include <Cori.hpp>

namespace AnimatorNames {
	inline constexpr char PlayerMainAnimator[] = "Player Main Animator";

	inline constexpr char PlayerParticleAnimator[] = "Player Particle Animator";

}

namespace Animations {
	namespace Player {
		inline constexpr Cori::AnimationDescriptor Idle(0, true, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor IdleTransition(17, false, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor DoubleJump(9, true, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor Run(2, true, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor WallSlide(22, true, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor WallJump(23, false, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor Fall(7, true, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor JumpStart(5, false, AnimatorNames::PlayerMainAnimator);

		inline constexpr Cori::AnimationDescriptor JumpMid(6, true, AnimatorNames::PlayerMainAnimator);

		namespace Particles {
			inline constexpr Cori::AnimationDescriptor RunFront(3, true, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor RunBack(4, true, AnimatorNames::PlayerParticleAnimator);

			inline constexpr Cori::AnimationDescriptor Jump(7, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor Landing(8, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor WallJump(14, false, AnimatorNames::PlayerParticleAnimator);
			inline constexpr Cori::AnimationDescriptor WallSlide(15, true, AnimatorNames::PlayerParticleAnimator);

			inline constexpr Cori::AnimationDescriptor DoubleJump(10, false, AnimatorNames::PlayerParticleAnimator);

		}
	}
}