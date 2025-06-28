#pragma once
#include <Cori.hpp>

namespace AnimatorNames {
	inline constexpr char TestAnimator[] = "Test";
}

namespace Animations {
	namespace Player {
		using Idle = Cori::AnimationHandle<0 , true, AnimatorNames::TestAnimator>;
		using DoubleJump = Cori::AnimationHandle<9, false, AnimatorNames::TestAnimator>;
		using Run = Cori::AnimationHandle<2, true, AnimatorNames::TestAnimator>;
		using WallSlide = Cori::AnimationHandle<22, true, AnimatorNames::TestAnimator>;


		using Fall = Cori::AnimationHandle<7, true, AnimatorNames::TestAnimator>;

		using WallSlide = Cori::AnimationHandle<22, true, AnimatorNames::TestAnimator>;
		using WallJump = Cori::AnimationHandle<14, false, AnimatorNames::TestAnimator>;

		using JumpStart = Cori::AnimationHandle<5, false, AnimatorNames::TestAnimator>;
		using JumpMid = Cori::AnimationHandle<6, true, AnimatorNames::TestAnimator>;
	}
}