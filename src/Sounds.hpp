#pragma once
#include <Cori.hpp>

namespace Sounds {
	namespace Player {
		inline const Cori::Audio::Sound::Descriptor Step1{
			"Step 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/07_Step_rock_01.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Step2{
			"Step 2",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/08_Step_rock_02.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Step3{
			"Step 3",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/09_Step_rock_03.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Jump1{
			"Jump 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/28_Jump_01.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Jump2{
			"Jump 2",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/29_Jump_02.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Jump3{
			"Jump 3",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/30_Jump_03.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Jump4{
			"Jump 4",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/31_Jump_04.wav"
		};

		inline const Cori::Audio::Sound::Descriptor DoubleJump1{
			"Double Jump 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/33_Double_jump_01.wav"
		};

		inline const Cori::Audio::Sound::Descriptor DoubleJump2{
			"Double Jump 2",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/34_Double_jump_02.wav"
		};

		inline const Cori::Audio::Sound::Descriptor DoubleJump3{
			"Double Jump 3",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/35_Double_jump_03.wav"
		};

		inline const Cori::Audio::Sound::Descriptor DoubleJump4{
			"Double Jump 4",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/36_Double_jump_04.wav"
		};

		inline const Cori::Audio::Sound::Descriptor FallingLoop1{
			"Falling Loop 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/43_Falling_01_Loop.wav"
		};

		inline const Cori::Audio::Sound::Descriptor FallingLoop2{
			"Falling Loop 2",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/44_Falling_02_Loop.wav"
		};

		//inline const Cori::Audio::Sound::Descriptor Landing1{
		//	"Landing 1",
		//	Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/45_Landing_01.wav"
		//};

		inline const Cori::Audio::Sound::Descriptor Landing2{
			"Landing 2",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/46_Landing_02.wav"
		};

		inline const Cori::Audio::Sound::Descriptor Landing3{
			"Landing 3",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/47_Landing_03.wav"
		};

		inline const Cori::Audio::Sound::Descriptor SlidingLoop1{
			"Sliding Loop 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/20_Slide_02_Loop.wav"
		};

		inline const Cori::Audio::Sound::Descriptor SlidingLoop2{
			"Sliding Loop 1",
			Cori::FileSystem::PathManager::GetAliasedPath("SOUNDS") / "player/21_Slide_03_Loop.wav"
		};
	}
}