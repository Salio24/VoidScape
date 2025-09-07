#pragma once
#include <Cori.hpp>

namespace AnimationPacks {
	inline const Cori::Graphics::AnimationPack::Descriptor PlayerMovement {
		"Player Movement",
		"../../assets/textures/player/PlayerSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};

	inline const Cori::Graphics::AnimationPack::Descriptor PlayerMovementFX {
		"Player Movement FX",
		"../../assets/textures/player/FXSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};
}