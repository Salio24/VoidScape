#pragma once
#include <Cori.hpp>

namespace AnimationPacks {
	inline const Cori::Graphics::AnimationPack::Descriptor PlayerMovement {
		"Player Movement",
		Cori::FileSystem::PathManager::GetAliasedPath("TEXTURES") / "player/PlayerSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};

	inline const Cori::Graphics::AnimationPack::Descriptor PlayerMovementFX {
		"Player Movement FX",
		Cori::FileSystem::PathManager::GetAliasedPath("TEXTURES") / "player/FXSheet.json",
		Cori::Graphics::AnimationPack::ASEPRITE
	};

	inline const Cori::Graphics::AnimationPack::Descriptor CoinSpin {
		"Coin Spin",
		Cori::FileSystem::PathManager::GetAliasedPath("TEXTURES") / "level/CoinSpin.json",
		Cori::Graphics::AnimationPack::CORI_VARYING
	};
}