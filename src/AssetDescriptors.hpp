#pragma once
#include <Cori.hpp>

namespace Assets {
	inline const Cori::Graphics::Font::Descriptor GlobalFont {
		"Main Font",
		Cori::FileSystem::PathManager::GetAliasedPath("FONTS") / std::filesystem::path("definetlyaninvalidfont.forsure"),
		{ Cori::Graphics::Font::CharsetRanges::Latin, Cori::Graphics::Font::CharsetRanges::LatinExtendedA, Cori::Graphics::Font::CharsetRanges::LatinExtendedB },
	};

	inline const Cori::Graphics::SpriteAtlas::Descriptor Coin {
		"Coin Temp",
		Cori::FileSystem::PathManager::GetAliasedPath("TEXTURES") / "level/CoinSpin.png",
		{32, 32}
	};

	inline const Cori::Graphics::SpriteAtlas::Descriptor Door {
		"Door Temp",
		Cori::FileSystem::PathManager::GetAliasedPath("TEXTURES") / "level/DoorOpened.png",
		{64, 80}
	};
}