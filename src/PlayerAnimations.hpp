#pragma once
#include <Cori.hpp>

namespace AnimatorNames {
	inline constexpr char TestAnimator[] = "Test";
}

namespace Animations {
	using PlayerIdle = Cori::AnimationHandle<0 , true, AnimatorNames::TestAnimator>;
	using PlayerDoubleJump = Cori::AnimationHandle<9, false, AnimatorNames::TestAnimator>;
	using PlayerRun = Cori::AnimationHandle<2, true, AnimatorNames::TestAnimator>;
	using PlayerWallSlide = Cori::AnimationHandle<22, true, AnimatorNames::TestAnimator>;


	using PlayerFall = Cori::AnimationHandle<7, true, AnimatorNames::TestAnimator>;

	using PlayerWallSlide = Cori::AnimationHandle<22, true, AnimatorNames::TestAnimator>;
	using PlayerWallJump = Cori::AnimationHandle<14, false, AnimatorNames::TestAnimator>;

	using PlayerJumpStart = Cori::AnimationHandle<5, false, AnimatorNames::TestAnimator>;
	using PlayerJumpMid = Cori::AnimationHandle<6, true, AnimatorNames::TestAnimator>;



}