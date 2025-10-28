#pragma once
#include <Cori.hpp>
#include "Animations.hpp"
#include "AnimationPacks.hpp"
#include "TrackDefs.hpp"
#include "Sounds.hpp"

namespace States {
	namespace Player {
		class Run final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Run), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunFront), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp1 = *tmp;
							temp1.SetParent(player);
						}
					}
				}

				auto result2 = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result2) {
					auto system = result2->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::RunBack), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp2 = *tmp;
							temp2.SetParent(player);
						}
					}
				}

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::Steps);
				if (track) {
					constexpr Cori::Audio::PlayParams params = { .MaxMilliseconds = 400.0f, .LoopedInSequence = true };
					auto step1sound = Cori::AssetManager::Get(Sounds::Player::Step1);
					auto step2sound = Cori::AssetManager::Get(Sounds::Player::Step1);
					auto step3sound = Cori::AssetManager::Get(Sounds::Player::Step1);

					auto step1 = std::make_pair(step1sound, params);
					auto step2 = std::make_pair(step2sound, params);
					auto step3 = std::make_pair(step3sound, params);

					track.value()->Stop(true);
					track.value()->Play(step1, step2, step3);
				}

			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}

				if (player.HasComponents<Cori::World::Components::Entity::AudioSource>()) {

					auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
					auto track = as.GetTrack(Tracks::Player::Steps);
					if (track) {
						track.value()->Stop(false);
					}
				}

				if (temp1.IsValid() && temp2.IsValid()) {
					temp1.UnlinkFromParent();
					temp2.UnlinkFromParent();
					temp1.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(false);
					temp2.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(false);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "RunState";
			}

			Cori::World::Entity temp1;
			Cori::World::Entity temp2;
		};

		class Idle final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Idle), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "IdleState";
			}
		};

		class Jump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto jumpStart = std::make_pair(pack->GetAnimation(Animations::Player::JumpStart), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				const auto jumpMid = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(jumpStart, jumpMid);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Jump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
					}
				}

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::Jump);
				if (track) {
					constexpr Cori::Audio::PlayParams params = { .LoopedInSequence = false };
					uint32_t random = Cori::Utility::RandomUint32::Gen(1, 4);
					std::shared_ptr<Cori::Audio::Sound> sound;
					switch (random) {
						case 1:
							{
								sound = Cori::AssetManager::Get(Sounds::Player::Jump1);
								break;
							}
						case 2:
							{
								sound = Cori::AssetManager::Get(Sounds::Player::Jump2);
								break;
							}
						case 3:
							{
								sound = Cori::AssetManager::Get(Sounds::Player::Jump3);
								break;
							}
						case 4:
							{
								sound = Cori::AssetManager::Get(Sounds::Player::Jump4);
								break;
							}
					}

					auto jump = std::make_pair(sound, params);

					track.value()->Stop(true);
					track.value()->Play(jump);
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "JumpState";
			}
		};

		class Fall final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Fall), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
				m_FallDuration += Cori::Core::Application::GetGameTimer().GetTimestep();

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::Falling);
				if (track) {
					constexpr Cori::Audio::PlayParams params = {};
					uint32_t random = Cori::Utility::RandomUint32::Gen(1, 2);
					std::shared_ptr<Cori::Audio::Sound> sound;
					switch (random) {
					case 1:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::FallingLoop1);
							break;
						}
					case 2:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::FallingLoop2);
							break;
						}
					}

					auto falling = std::make_pair(sound, params);

					track.value()->SetGain(0.0f);
					track.value()->Stop(true);
					track.value()->Play(falling);
				}

			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
				m_FallDuration += Cori::Core::Application::GetGameTimer().GetTimestep();
				if (m_FallDuration > 0.25f) {
					auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
					auto track = as.GetTrack(Tracks::Player::Falling);
					if (track) {
						track.value()->SetGain(std::clamp(m_FallDuration / 4.0f, 0.0f, 1.0f));
					}
				}
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (nextStateType == typeid(Idle) || nextStateType == typeid(Jump) || nextStateType == typeid(Run)) {
					auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
					if (result) {
						auto system = result->GetSystem<Cori::World::Systems::Animation>();
						if (system) {
							auto locked = system->lock();
							const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
							const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::Landing), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
							auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
							locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
						}
					}

					if (player.HasComponents<Cori::World::Components::Entity::AudioSource>()) {
						auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
						auto track = as.GetTrack(Tracks::Player::Landing);
						if (track) {
							constexpr Cori::Audio::PlayParams params = { .LoopedInSequence = false };
							uint32_t random = Cori::Utility::RandomUint32::Gen(1, 2);
							std::shared_ptr<Cori::Audio::Sound> sound;
							switch (random) {
							case 1:
								{
									sound = Cori::AssetManager::Get(Sounds::Player::Landing2);
									break;
								}
							case 2:
								{
									sound = Cori::AssetManager::Get(Sounds::Player::Landing3);
									break;
								}
							}

							auto land = std::make_pair(sound, params);

							track.value()->Stop(true);
							track.value()->Play(land);
						}
					}
				}

				if (player.HasComponents<Cori::World::Components::Entity::AudioSource>()) {
					auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
					auto track = as.GetTrack(Tracks::Player::Falling);
					if (track) {
						track.value()->Stop(true);
					}
				}


				m_FallDuration = 0.0f;


			}

			float m_FallDuration;

			[[nodiscard]] const char* GetDebugName() const override {
				return "FallState";
			}
		};

		class DoubleJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::DoubleJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::DoubleJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						locked->PlayAnimation(tr.GetLocalPosition(), tr.GetLocalDepthOffset(), tr.GetLocalScale(), tr.GetLocalRotation(), FXanim);
					}
				}

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::DoubleJump);
				if (track) {
					constexpr Cori::Audio::PlayParams params = { .LoopedInSequence = false };
					uint32_t random = Cori::Utility::RandomUint32::Gen(1, 4);
					std::shared_ptr<Cori::Audio::Sound> sound;
					switch (random) {
					case 1:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::DoubleJump1);
							break;
						}
					case 2:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::DoubleJump2);
							break;
						}
					case 3:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::DoubleJump3);
							break;
						}
					case 4:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::DoubleJump4);
							break;
						}
					}

					auto doubleJump = std::make_pair(sound, params);

					track.value()->Stop(true);
					track.value()->Play(doubleJump);
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "DoubleJumpState";
			}
		};

		class WallJump final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallJump), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = false });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallJump), Cori::Graphics::Animation::PlayParams{.LoopedInSequence = false});
						auto& tr = player.GetComponents<Cori::World::Components::Entity::Transform>();
						glm::vec2 scale = tr.GetLocalScale();
						glm::vec2 pos = tr.GetLocalPosition();
						locked->PlayAnimation({ pos.x - FXanim.first.GetFrameSize().x / 16.0f * Cori::Math::Sign(scale.x), pos.y }, tr.GetLocalDepthOffset(), { -scale.x, scale.y }, tr.GetLocalRotation(), FXanim);
					}
				}

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::WallJump);
				if (track) {
					constexpr Cori::Audio::PlayParams params = { .LoopedInSequence = false };
					uint32_t random = Cori::Utility::RandomUint32::Gen(1, 4);
					std::shared_ptr<Cori::Audio::Sound> sound;
					switch (random) {
					case 1:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::Jump1);
							break;
						}
					case 2:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::Jump2);
							break;
						}
					case 3:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::Jump3);
							break;
						}
					case 4:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::Jump4);
							break;
						}
					}

					auto jump = std::make_pair(sound, params);

					track.value()->Stop(true);
					track.value()->Play(jump);
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "WallJumpState";
			}
		};

		class WallSlide final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);

				auto result = Cori::World::SceneManager::GetHandle(player.GetOwnerSceneID());
				if (result) {
					auto system = result->GetSystem<Cori::World::Systems::Animation>();
					if (system) {
						auto locked = system->lock();
						const auto FXpack = Cori::AssetManager::Get(AnimationPacks::PlayerMovementFX);
						const auto FXanim = std::make_pair(FXpack->GetAnimation(Animations::Player::Particles::WallSlide), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
						auto tmp = locked->PlayAnimation({ 0.0f, 0.0f }, -1, { 1.0f, 1.0f }, 0.0f, FXanim);
						if (tmp) {
							temp = *tmp;
							temp.SetParent(player);
						}
					}
				}

				auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
				auto track = as.GetTrack(Tracks::Player::WallSlide);
				if (track) {
					constexpr Cori::Audio::PlayParams params = {};
					uint32_t random = Cori::Utility::RandomUint32::Gen(1, 2);
					std::shared_ptr<Cori::Audio::Sound> sound;
					switch (random) {
					case 1:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::SlidingLoop1);
							break;
						}
					case 2:
						{
							sound = Cori::AssetManager::Get(Sounds::Player::SlidingLoop2);
							break;
						}
					}

					auto slide = std::make_pair(sound, params);

					track.value()->Stop(true);
					track.value()->Play(slide);
				}
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}

				if (player.HasComponents<Cori::World::Components::Entity::AudioSource>()) {
					auto& as = player.GetComponents<Cori::World::Components::Entity::AudioSource>();
					auto track = as.GetTrack(Tracks::Player::WallSlide);
					if (track) {
						track.value()->Stop(true);
					}
				}

				if (temp.IsValid()) {
					temp.GetComponents<Cori::World::Components::Entity::QuadAnimator>().Stop(true);
					temp.UnlinkFromParent();
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "WallSlideState";
			}

			Cori::World::Entity temp;
		};

		// ascending state doesn't always gets triggered when player goes "up", most of the time it is some of the jump states
		// this one exists only to change animation when player was touching the wall but now he is above it and didn't move to one of the sides
		class Ascending final : public Cori::World::EntityState {
		public:
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::JumpMid), Cori::Graphics::Animation::PlayParams{ .LoopedInSequence = true });
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {

			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
				if (player.HasComponents<Cori::World::Components::Entity::QuadAnimator>()) {
					auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
					ar.Stop(true);
				}
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "AscendingState";
			}
		};

		class Dead final : public Cori::World::EntityState {
			void OnEnter(Cori::World::Entity& player, const std::type_index& lastStateType) override {
				const auto pack = Cori::AssetManager::Get(AnimationPacks::PlayerMovement);

				auto& ar = player.GetComponents<Cori::World::Components::Entity::QuadAnimator>();
				const auto anim = std::make_pair(pack->GetAnimation(Animations::Player::Dead), Cori::Graphics::Animation::PlayParams{});
				ar.Play(anim);
			}

			void OnTickUpdate(Cori::World::Entity& player, float deltaTime) override {
			}

			void OnExit(Cori::World::Entity& player, const std::type_index& nextStateType) override {
			}

			[[nodiscard]] const char* GetDebugName() const override {
				return "Dead";
			}
		};
	}

}
