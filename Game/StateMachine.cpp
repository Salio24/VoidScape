#include "StateMachine.hpp"

StateMachine::StateMachine() {

}

StateMachine::~StateMachine() {

}


bool StateMachine::CheckPlayerStateChange() {
	return currentPlayerState != lastState;
}

bool StateMachine::CompareToLastState(const PlayerStates& state) {
	return state == lastState;
}

void StateMachine::CheckPlayerState(Actor& actor, MovementHandler& movementHandler) {
	lastState = currentPlayerState;
	if (!movementHandler.isGrounded && !movementHandler.canWallStick && movementHandler.canDoubleJump) {
		if (actor.velocity.y > 0.0f) {
			currentPlayerState = PlayerStates::JUMPING;
			// jump
		}
		else if (actor.velocity.y <= 0.0f) {
			currentPlayerState = PlayerStates::FALLING;
			// fall
		}
	}
	else if (!movementHandler.isGrounded && !movementHandler.canWallStick && !movementHandler.canDoubleJump) {
		// double jump
		if (actor.velocity.y > 0.0f) {
			currentPlayerState = PlayerStates::DOUBLE_JUMPING;
			// jump
		}
		else if (actor.velocity.y <= 0.0f) {
			currentPlayerState = PlayerStates::FALLING;
			// fall
		}
	}
	else if (!movementHandler.isGrounded && movementHandler.canWallStick) {
		currentPlayerState = PlayerStates::WALLSLIDING;
		// wall slide
	}
	else {
		if (!(actor.velocity.x < 1.0f && actor.velocity.x > -1.0f) && (movementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] || movementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)])) {
			currentPlayerState = PlayerStates::RUNNING;
			// run
		}
		else {
			currentPlayerState = PlayerStates::IDLE;
			// idle
		}
	}
	if (movementHandler.isSliding && movementHandler.isGrounded) {
		currentPlayerState = PlayerStates::SLIDING;
		// slide
	}
	if (movementHandler.isSlamming) {
		currentPlayerState = PlayerStates::SLAMMING;
		// slam
	} 
	if (actor.mDead) {
		currentPlayerState = PlayerStates::DEAD;
	}
	if (actor.isSucked || actor.isSuckedPortal) {
		currentPlayerState = PlayerStates::SUCKED;
	}
	if (actor.isConsumedByVoid) {
		currentPlayerState = PlayerStates::VOID_CONSUMED;
	}
	if (actor.mEscaped) {
		currentPlayerState = PlayerStates::ESCAPED;
	}
}

void StateMachine::Update(MovementHandler& movementHandler, AnimationHandler& animationHandler, AudioHandler& audioHandler, Actor& actor, const float& deltaTime) {
	CheckPlayerState(actor, movementHandler);
	switch (movementHandler.lookDirection) {
	case LookDirections::LEFT:
		mActorFlipped = true;
		break;
	case LookDirections::RIGHT:
		mActorFlipped = false;
		break;
	default:
		break;
	}

	switch (currentPlayerState) {
	case PlayerStates::RUNNING:
		if (runAnimationOneShot) {
			animationHandler.RunAnimation.AnimationTimer = std::chrono::high_resolution_clock::now();
			animationHandler.RunAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			runAnimationOneShot = false;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animationHandler.RunAnimation.AnimationTimer).count() > animationHandler.RunAnimation.AnimationTime + deltaTime * 1000) {
			animationHandler.RunAnimation.AnimationTimer = std::chrono::high_resolution_clock::now();
			animationHandler.RunAnimation.index = 0;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animationHandler.RunAnimation.SingleFrameTimer).count() > animationHandler.RunAnimation.SingleFrameTime + deltaTime * 1000) {
			animationHandler.RunAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			animationHandler.RunAnimation.index++;
		}
		if (animationHandler.RunAnimation.index > animationHandler.RunAnimation.AnimationTextureIndexes.size() - 1) {
			animationHandler.RunAnimation.index = 0;
		}
		if ((animationHandler.RunAnimation.index == 1 || animationHandler.RunAnimation.index == 2 || animationHandler.RunAnimation.index == 4 || animationHandler.RunAnimation.index == 5) && playingRunSound == false) {
			playingRunSound = true;
		}

		if ((animationHandler.RunAnimation.index == 3 || animationHandler.RunAnimation.index == 0) && playingRunSound == true) {
			playingRunSound = false;
			audioHandler.PlayNextStepSound();
		}

		mCurrentActorDrawSize = animationHandler.RunAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.RunAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.RunAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.RunAnimation.AnimationTextureIndexes[animationHandler.RunAnimation.index];
		break;
	case PlayerStates::JUMPING:
		mCurrentActorDrawSize = animationHandler.JumpAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.JumpAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.JumpAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.JumpAnimation.AnimationTextureIndexes[0];

		if (CheckPlayerStateChange()) {
			Mix_PlayChannel(2, audioHandler.Jump, 0);
		}
		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::DOUBLE_JUMPING:
		mCurrentActorDrawSize = animationHandler.JumpAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.JumpAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.JumpAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.JumpAnimation.AnimationTextureIndexes[0];
		if (CheckPlayerStateChange()) {
			Mix_PlayChannel(3, audioHandler.DoubleJump, 0);
		}
		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::FALLING:
		mCurrentActorDrawSize = animationHandler.FallAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.FallAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.FallAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.FallAnimation.AnimationTextureIndexes[0];

		if (!Mix_Playing(4)) {
			Mix_PlayChannel(4, audioHandler.WindSoft, 0);
		}

		if (FallVolumeTimer >= FallVolumeTime) {
			FallVolumeTimer = 0.0f;
			FallVolume += 10;
			Mix_Volume(4, FallVolume);
		}
		else {
			FallVolumeTimer += deltaTime;
		}
		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::SLIDING:
		mCurrentActorDrawSize = animationHandler.SlideAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.SlideAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.SlideAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.SlideAnimation.AnimationTextureIndexes[0];

		if (!Mix_Playing(5)) {
			Mix_PlayChannel(5, audioHandler.Slide, 0);
		}
		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::SLAMMING:
		mCurrentActorDrawSize = animationHandler.SlamAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.SlamAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.SlamAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.SlamAnimation.AnimationTextureIndexes[0];

		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::WALLSLIDING:
		mCurrentActorDrawSize = animationHandler.WallSlideAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.WallSlideAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.WallSlideAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.WallSlideAnimation.AnimationTextureIndexes[0];
		mActorFlipped = !mActorFlipped;


		if (!Mix_Playing(6)) {
			Mix_PlayChannel(6, audioHandler.WallSlide, 0);
		}
		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::DEAD:
		if (deadAnimOneShot) {
			animationHandler.DeadAnimation.AnimationTimer = std::chrono::high_resolution_clock::now();
			animationHandler.DeadAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			animationHandler.DeadAnimation.index = 0;
			deadAnimOneShot = false;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animationHandler.DeadAnimation.AnimationTimer).count() > animationHandler.DeadAnimation.AnimationTime + deltaTime * 1000) {
			deadAnimDone = true;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animationHandler.DeadAnimation.SingleFrameTimer).count() > animationHandler.DeadAnimation.SingleFrameTime + deltaTime * 1000) {
			if (!deadAnimDone) {
				animationHandler.DeadAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
				animationHandler.DeadAnimation.index++;
			}
		}
		if (animationHandler.DeadAnimation.index > animationHandler.DeadAnimation.AnimationTextureIndexes.size() - 1) {
			animationHandler.DeadAnimation.index = 0;
		}

		mCurrentActorDrawSize = animationHandler.DeadAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.DeadAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.DeadAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.DeadAnimation.AnimationTextureIndexes[animationHandler.DeadAnimation.index];


		break;
	case PlayerStates::HIT:

		break;
	case PlayerStates::IDLE:
		mCurrentActorDrawSize = animationHandler.IdleAnimation.Size * actor.mSizeMultiplier;
		mCurrentActorTextureSize = animationHandler.IdleAnimation.Size;
		mCurrentActorTexturePosition = animationHandler.IdleAnimation.TexturePosition;
		mCurrentActorTextureIndex = animationHandler.IdleAnimation.AnimationTextureIndexes[0];

		animationHandler.RunAnimation.index = 0;
		runAnimationOneShot = true;
		break;
	case PlayerStates::SUCKED:

		if (!Mix_Playing(10)) {
			Mix_PlayChannel(10, audioHandler.WindHard, 0);
		}

		if (SuckedVolumeTimer >= SuckedVolumeTime) {
			SuckedVolumeTimer = 0.0f;
			SuckedVolume += 7;
			Mix_Volume(10, SuckedVolume);
		}
		else {
			SuckedVolumeTimer += deltaTime;
		}

		break;
	case PlayerStates::VOID_CONSUMED:
		break;
	case PlayerStates::ESCAPED:
		break;
	default:
		break;
	}

	if (CheckPlayerStateChange()) {
		if (CompareToLastState(PlayerStates::SLAMMING) && currentPlayerState != PlayerStates::SLAMMING) {
			audioHandler.PlayNextLandHardSound();
		}

		if (CompareToLastState(PlayerStates::FALLING) && currentPlayerState != PlayerStates::FALLING && currentPlayerState != PlayerStates::SLAMMING && currentPlayerState != PlayerStates::DOUBLE_JUMPING && currentPlayerState != PlayerStates::DEAD && currentPlayerState != PlayerStates::VOID_CONSUMED) {
			audioHandler.PlayNextLandSoftSound();
		}

		if (currentPlayerState != PlayerStates::SLIDING) {
			Mix_HaltChannel(5);
		}
		if (currentPlayerState != PlayerStates::WALLSLIDING) {
			Mix_HaltChannel(6);
		}
		if (currentPlayerState != PlayerStates::FALLING) {
			Mix_HaltChannel(4);
			Mix_Volume(4, 0);
			FallVolume = 1;
		}
		if (currentPlayerState != PlayerStates::SUCKED) {
			Mix_HaltChannel(10);
			Mix_Volume(10, 0);
			SuckedVolume = 1;
		}
	}
}