#pragma once
#include <Cori.hpp>

namespace AnimationPacks {
	inline const Cori::AnimationPackDescriptor PlayerMovement {
		"Player Movement",
		"../../assets/textures/player/PlayerSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};

	inline const Cori::AnimationPackDescriptor PlayerMovementFX {
		"Player Movement FX",
		"../../assets/textures/player/FXSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};
}