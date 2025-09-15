#pragma once
#include <Cori.hpp>
#include <PathDefinesGenerated.hpp>

namespace Assets {
	inline const Cori::Graphics::Font::Descriptor GlobalFont {
		"Main Font",
		GetVoidScapeRootDir() / std::filesystem::path("assets/fonts/definetlyaninvalidfont.forsure"),
		{ Cori::Graphics::Font::CharsetRanges::Latin, Cori::Graphics::Font::CharsetRanges::LatinExtendedA, Cori::Graphics::Font::CharsetRanges::LatinExtendedB },
	};

	inline const Cori::Graphics::SpriteAtlas::Descriptor Coin {
		"Coin Temp",
		GetVoidScapeRootDir() / "assets/textures/level/CoinSpin.png",
		{32, 32}
	};

	inline const Cori::Graphics::SpriteAtlas::Descriptor Door {
		"Door Temp",
		GetVoidScapeRootDir() / "assets/textures/level/DoorOpened.png",
		{64, 80}
	};
}