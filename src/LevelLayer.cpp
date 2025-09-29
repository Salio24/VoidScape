#include "LevelLayer.hpp"
#include "Triggers/Orb.hpp"
#include "Triggers/EscapeDoor.hpp"
#include "Components.hpp"
#include "Player/States.hpp"
#include "Tags.hpp"
#include "Triggers/Trap.hpp"
#include "Player/AnimationPacks.hpp"
#include "EntityNameDefs.hpp"
#include "AssetDescriptors.hpp"
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include "Systems/MovingPlatform.hpp"

static bool manualStep = false;

LevelLayer::LevelLayer() : Layer("Level Layer") {
	const auto success = Cori::World::SceneManager::CreateScene("Level Scene");
	CORI_ASSERT(success, "Failed to create Level Scene. Error: {}", success.error().what());
	const auto success_ = BindScene("Level Scene");
	CORI_ASSERT(success_, "Failed to bind Level Scene. Error: {}", success_.error().what());

	const int screenWidth = Cori::Core::Application::GetWindow().GetWidth();
	const int screenHeight = Cori::Core::Application::GetWindow().GetHeight();

	ActiveScene.GetActiveCamera().CreateOrthoCamera(0, static_cast<float>(screenWidth) / (static_cast<float>(screenHeight) / 360.0f), 0, 360);

	ActiveScene.RegisterSystem<Systems::MovingPlatform>();
}

LevelLayer::~LevelLayer() {
	ActiveScene.DestroyEntity(m_Player);
}

void LevelLayer::OnAttach() {

}

void LevelLayer::OnDetach() {
}

void LevelLayer::OnUpdate(Cori::Core::GameTimer& gameTimer) {
	if (m_LevelLoaded) {
		const auto& hp = m_Player.GetComponents<Components::Health>();

		const auto& fsm = m_Player.GetComponents<Cori::World::Components::Entity::StateMachine>();

		static std::string timerText;
		glm::mat3 textPos;
		Cori::Graphics::Renderer2D::TextAlignment alignment = Cori::Graphics::Renderer2D::RIGHT;

		if (!fsm.IsInState<States::Player::Dead>() && !m_LevelCompleted) {
			alignment = Cori::Graphics::Renderer2D::LEFT;
			textPos = glm::translate(glm::mat3(1.0f), glm::vec2(15.0f, 330.0f));
			timerText = Cori::Core::GameTimer::FormatTime_S_to_M_S_MS(hp.m_TimeHealth);
		}
		else if (fsm.IsInState<States::Player::Dead>()) {
			alignment = Cori::Graphics::Renderer2D::CENTER;
			textPos = glm::translate(glm::mat3(1.0f), ActiveScene.GetActiveCamera().GetSize() / 2.0f);
			timerText = "Skill Issue. One reddit user says \"Kill yourself\"";
		}
		else if (m_LevelCompleted) {
			alignment = Cori::Graphics::Renderer2D::CENTER;
			textPos = glm::translate(glm::mat3(1.0f), ActiveScene.GetActiveCamera().GetSize() / 2.0f);
			timerText = std::format("You escaped, now do it faster. \nTime left in the bank: {}", Cori::Core::GameTimer::FormatTime_S_to_M_S_MS(hp.m_TimeHealth));
		}

		Cori::Graphics::Renderer2D::SubmitText(Cori::Graphics::Renderer2D::SCREEN_SPACE, alignment, textPos, 20, timerText, glm::vec4(1.0f), Cori::AssetManager::Get(Assets::GlobalFont).get(), 20, 1000.0f, 0.0f, 0.0f);

		auto& mover = m_Player.GetComponents<Components::Mover>();

		mover.OnUpdate(gameTimer.GetDeltaTime(), gameTimer.GetTickAlpha());
		m_MainCamera.OnUpdate(gameTimer, ActiveScene.GetActiveCamera(), !m_LevelCompleted);
	}
}

void LevelLayer::OnTickUpdate(Cori::Core::GameTimer& gameTimer) {
	if (m_LevelLoaded) {
		auto& hp = m_Player.GetComponents<Components::Health>();
		hp.OnTickUpdate(gameTimer.GetTimestep(), m_LevelCompleted);

		const auto& fsm = m_Player.GetComponents<Cori::World::Components::Entity::StateMachine>();

		auto& mover = m_Player.GetComponents<Components::Mover>();

		mover.OnTickUpdate(gameTimer.GetTimestep(),m_MainCamera, !fsm.IsInState<States::Player::Dead>() && !m_LevelCompleted);

		const glm::vec2 playerPos = m_Player.GetComponents<Cori::World::Components::Entity::Transform>().GetLocalPosition();
		const glm::vec2 playerHalfSize = m_Player.GetComponents<Cori::World::Components::Entity::QuadRenderer>().GetHalfSize();
		m_MainCamera.OnTickUpdate(gameTimer, playerPos, Cori::Physics::ToPixels(mover.m_RelativeVelocity) ,ActiveScene.GetActiveCamera());
	}
}

void LevelLayer::OnImGuiRender(Cori::Core::GameTimer& gameTimer) {
	if (m_LevelLoaded) {
		if (m_PhysicsDebugDraw) {
			Cori::ImGuiPresets::Box2dDebugDraw(ActiveScene.GetActiveCamera().GetSize(), ActiveScene.GetActiveCamera().GetPosition(), CORI_PIXELS_PER_METER, this, true, 2000.0f);
		}

		ImGui::Begin("Layer Layer UI");

		auto& fsm = m_Player.GetComponents<Cori::World::Components::Entity::StateMachine>();
		if (fsm.IsInState<States::Player::Dead>() || m_LevelCompleted) {
			if (ImGui::Button("Restart")) {
				m_Player.SetActive(true);
				fsm.SetState<States::Player::Idle>();
				auto& mover = m_Player.GetComponents<Components::Mover>();
				mover.ResetState();
				mover.TeleportToSpawn();
				auto view = ActiveScene.View<Cori::World::Components::Entity::Trigger>();
				for (auto entity : view) {
					entity.SetActive(true);
				}
				m_LevelCompleted = false;
				auto& hp = m_Player.GetComponents<Components::Health>();
				hp.Reset();
				auto system = ActiveScene.GetSystem<Systems::MovingPlatform>();

				if (system) {
					auto locked = system->lock();
					locked->Reset(gameTimer);
				}
			}
		}

		if (ImGui::Button("Reset Platforms")) {
			auto system = ActiveScene.GetSystem<Systems::MovingPlatform>();

			if (system) {
				auto locked = system->lock();
				locked->Reset(gameTimer);
			}
		}

		if (ImGui::Checkbox("Box2d debug draw", &m_PhysicsDebugDraw)) {
			if (!m_PhysicsDebugDraw) {
				m_MoverDebugDraw = false;
			}
		}
		if (m_PhysicsDebugDraw) {
			ImGui::Checkbox("Mover debug draw", &m_MoverDebugDraw);
		}
		if (ImGui::Checkbox("Manual Step(disable, K - step)", &manualStep)) {
			Cori::Core::Application::GetGameTimer().SetManualTickStep(manualStep);
		}

		if (ImGui::Button("Add dynamic box")) {
			auto ent = ActiveScene.CreateEntity("Dynamic Box", Tags::ForTest);

			Cori::Physics::Body::Params bp;
			bp.type = b2_dynamicBody;
			bp.position = { 4.0f, 4.0f };

			auto& rb = ent.AddComponent<Cori::World::Components::Entity::RigidBody>(ActiveScene.GetPhysicsWorld(), bp, ent);

			Cori::Physics::Shape::Params sp;

			rb.CreateShape(Cori::Physics::DestroyWithParent, sp, Cori::Physics::Polygon::CreateBox({ 1.0f, 1.0f }));
		}

		ImGui::SeparatorText("Camera Settings");

		ImGui::SliderFloat("Asymptotic Average X", &m_MainCamera.m_AsymptoticAverageX, 0.01f, 1.0f, "%.2f");
		ImGui::SliderFloat("Asymptotic Average Y", &m_MainCamera.m_AsymptoticAverageY, 0.01f, 1.0f, "%.2f");
		ImGui::SliderFloat("Velocity Dep Mod x", &m_MainCamera.m_VelocityDependencyModifierX, -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Velocity Dep Mod Y", &m_MainCamera.m_VelocityDependencyModifierY, -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("m_MaxTransformShakeX", &m_MainCamera.m_MaxTransformShakeX, 0.0f, 1000.0f, "%.2f");
		ImGui::SliderFloat("m_MaxTransformShakeY", &m_MainCamera.m_MaxTransformShakeY, 0.0f, 1000.0f, "%.2f");
		ImGui::SliderFloat("m_MaxRotationalShake", &m_MainCamera.m_MaxRotationalShake, 0.0f, 1000.0f, "%.2f");
		ImGui::SliderFloat("m_TransformShakeFrequencyModifier", &m_MainCamera.m_TransformShakeFrequencyModifier, 0.01f, 2.0f, "%.2f");
		ImGui::SliderFloat("m_RotationalShakeFrequencyModifier", &m_MainCamera.m_RotationalShakeFrequencyModifier, 0.01f, 2.0f, "%.2f");

		static float scale = 1.0f;

		ImGui::Separator();

		if (ImGui::SliderFloat("Camera Scale", &scale, -0.25, 4.0f, "%.2f")) {
			ActiveScene.GetActiveCamera().SetScale(glm::vec2(scale));
			ActiveScene.GetActiveCamera().RecalculateVP();
		}

		if (ImGui::Button("Add 0.5 trauma")) {
			m_MainCamera.AddTrauma(0.5f);
		}

		if (ImGui::Button("Add 0.1 trauma")) {
			m_MainCamera.AddTrauma(0.1f);
		}

		ImGui::End();

		if (m_MoverDebugDraw) {
			auto& mover = m_Player.GetComponents<Components::Mover>();
			mover.DebugDraw(gameTimer.GetTimestep());
			mover.UpdateGui();
		}
	}
}

void LevelLayer::OnEvent(Cori::Core::Event& event) {
	Cori::Core::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<Cori::Core::WindowResizeEvent>([this](const Cori::Core::WindowResizeEvent& e) -> bool {
		ActiveScene.GetActiveCamera().CreateOrthoCamera(0, static_cast<float>(e.GetWidth()) / (static_cast<float>(e.GetHeight()) / 360.0f), 0, 360);
		return false;
	});

	dispatcher.Dispatch<Events::PlayerDied>([](const Events::PlayerDied& e) -> bool {
		return false;
	});

	dispatcher.Dispatch<Events::PlayerEscaped>([this](const Events::PlayerEscaped& e) -> bool {
		m_LevelCompleted = true;
		return false;
	});

	dispatcher.Dispatch<Events::PlayerTookDamage>([this](const Events::PlayerTookDamage& e) -> bool {
		if (e.GetDesiredCameraTrauma() == -1.0f) {
			m_MainCamera.AddTrauma(e.GetDamageAmount() / 360.0f);
		} else {
			m_MainCamera.AddTrauma(e.GetDesiredCameraTrauma());
		}

		return false;
	});
}

void LevelLayer::CreatePlayer(const float startingTime, const glm::vec2 spawnPos) {
	m_Player = ActiveScene.CreateEntity(EntityNames::PlayerRoot, Tags::Character);

	Cori::AssetManager::Preload({AnimationPacks::PlayerMovement, AnimationPacks::PlayerMovementFX});

	m_Player.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
	m_Player.AddComponent<Cori::World::Components::Entity::QuadAnimator>(m_Player);
	m_Player.AddComponent<Components::Spawnpoint>(spawnPos);
	{
		auto& transform = m_Player.GetComponents<Cori::World::Components::Entity::Transform>();
		transform.SetLocalDepth(4);
	}

	Cori::World::Entity playerParticles1 = ActiveScene.CreateEntity("Movement Particles Part 1", Tags::Character);
	playerParticles1.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
	playerParticles1.AddComponent<Cori::World::Components::Entity::QuadAnimator>(playerParticles1);
	CORI_CHECK_EXPECTED(playerParticles1.SetParent(m_Player));
	playerParticles1.SetActive(false);
	{
		auto& transform = playerParticles1.GetComponents<Cori::World::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	Cori::World::Entity playerParticles2 = ActiveScene.CreateEntity("Movement Particles Part 2", Tags::Character);
	playerParticles2.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
	playerParticles2.AddComponent<Cori::World::Components::Entity::QuadAnimator>(playerParticles2);
	CORI_CHECK_EXPECTED(playerParticles2.SetParent(m_Player));
	playerParticles2.SetActive(false);
	{
		auto& transform = playerParticles2.GetComponents<Cori::World::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	Cori::World::Entity playerParticlesIndependent = ActiveScene.CreateEntity("Movement Particles Independent Root", Tags::Character);
	CORI_CHECK_EXPECTED(playerParticlesIndependent.SetParent(m_Player));
	{
		auto& transform = playerParticlesIndependent.GetComponents<Cori::World::Components::Entity::Transform>();
		transform.SetLocalDepth(-1);
	}

	{
		Cori::World::Entity particles = ActiveScene.CreateEntity("Landing Particles", Tags::Character);
		particles.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::World::Components::Entity::QuadAnimator>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::World::Entity particles = ActiveScene.CreateEntity("Jumping Particles", Tags::Character);
		particles.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::World::Components::Entity::QuadAnimator>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::World::Entity particles = ActiveScene.CreateEntity("WallJump Particles", Tags::Character);
		particles.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::World::Components::Entity::QuadAnimator>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}

	{
		Cori::World::Entity particles = ActiveScene.CreateEntity("DoubleJump Particles", Tags::Character);
		particles.AddComponent<Cori::World::Components::Entity::QuadRenderer>();
		particles.AddComponent<Cori::World::Components::Entity::QuadAnimator>(particles);
		CORI_CHECK_EXPECTED(particles.SetParent(playerParticlesIndependent));
		particles.SetActive(false);
	}


	auto& fsm = m_Player.AddComponent <Cori::World::Components::Entity::StateMachine>(m_Player);

	fsm.Register<States::Player::Idle>();
	fsm.SetState<States::Player::Idle>();
	fsm.Register<States::Player::Run>();
	fsm.Register<States::Player::Fall>();
	fsm.Register<States::Player::Jump>();
	fsm.Register<States::Player::DoubleJump>();
	fsm.Register<States::Player::WallJump>();
	fsm.Register<States::Player::WallSlide>();
	fsm.Register<States::Player::Ascending>();
	fsm.Register<States::Player::Dead>();
	fsm.SetState<States::Player::Idle>();

	m_Player.AddComponent<Components::Health>(startingTime, m_Player);

	Components::Mover::Params mp;
	mp.gravityDefault = 34.5f;

	m_Player.AddComponent<Components::Mover>(Cori::Physics::Capsule::Create({ 0.0f, -0.5f }, { 0.0f, 0.55f }, 0.37f), Cori::Physics::Capsule::Create({ 0.0f, -0.9f }, { 0.0f, 0.6f }, 0.45f), ActiveScene.GetPhysicsWorld(), m_Player, mp);
}

void LevelLayer::CreateEscapeDoor(const Cori::Physics::Vec2 pos) {
	auto tr = ActiveScene.CreateEntity("Escape Door", Tags::Triggers);

	Cori::Physics::Vec2 sizem = Cori::Physics::ToMeters(Assets::Door.m_SpriteResolution);
	Cori::Physics::Body::Params bp;
	bp.type = b2_staticBody;
	bp.position = pos + Cori::Physics::Vec2(0.0f, sizem.y / 2.0f - 1);
	bp.name = "Escape Door";

	auto& rb = tr.AddComponent<Cori::World::Components::Entity::RigidBody>(ActiveScene.GetPhysicsWorld(), bp, tr);

	Cori::Physics::Shape::Params spa;
	spa.filter.categoryBits = Cori::Physics::CollisionBits::SensorBit;
	spa.isSensor = true;
	spa.enableSensorEvents = true;

	rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Polygon::CreateBox({ sizem.x / 4.0f, sizem.y / 3.4f }));

	auto& trig = tr.AddComponent<Cori::World::Components::Entity::Trigger>(tr);
	trig.SetBehavior<Triggers::EscapeDoor>();

	const auto atlas = Cori::AssetManager::Get(Assets::Door);
	auto& trtr = tr.GetComponents<Cori::World::Components::Entity::Transform>();
	trtr.SetLocalPosition(Cori::Physics::ToPixels(pos) + glm::vec2(0.0f, Assets::Door.m_SpriteResolution.y / 2 - 16));
	trtr.SetLocalDepth(3);

	tr.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{ Assets::Door.m_SpriteResolution.x / 2.0f, Assets::Door.m_SpriteResolution.y / 2.0f}, atlas->GetTexture(), atlas->GetSpriteUVsAtIndex(0));
}

void LevelLayer::LoadLevel(const std::filesystem::path& path) {
	constexpr int blockSize = CORI_PIXELS_PER_METER;

	auto GridPosToPixels = [blockSize](const glm::ivec2 gridPos, const glm::ivec2 layerSize, const bool returnCentered) -> glm::vec2 {
		if (returnCentered) {
			return { gridPos.x * blockSize + blockSize / 2.0f, (layerSize.y - gridPos.y) * blockSize - blockSize / 2.0f };
		}

		return { gridPos.x * blockSize, (layerSize.y - gridPos.y) * blockSize - blockSize};
	};

	auto TiledPosToPixels = [](const tmx::Vector2f pos, const glm::ivec2 layerSize) -> glm::vec2 {
		return { pos.x, layerSize.y - pos.y };
	};

	tmx::Map map;

	std::vector<std::pair<uint32_t, uint32_t>> GDIs;

	std::vector<std::shared_ptr<Cori::Graphics::SpriteAtlas>> SpriteAtlases;
	std::unordered_map<std::string, std::shared_ptr<Cori::Graphics::SpriteAtlas>> SpriteAtlasMap;

	if (map.load(path.string())) {
		//auto gridSize = map.getTileSize();
		auto mapProperties = map.getProperties();

		float orbBonus = 5.0f;
		float initialPlayerTime = 100.0f;

		for (const auto& property : mapProperties) {
			if (property.getName() == "InitialPlayerTime") {
				initialPlayerTime = property.getFloatValue();
			}
			else if (property.getName() == "RegularOrbBonus") {
				orbBonus = property.getFloatValue();
			}
		}

		bool playerSpawnFound = false;

		bool escapeFound = false;

		tmx::Vector2u mapSize;
		{
			const auto mapSize_ = map.getBounds();
			mapSize.x = mapSize_.width;
			mapSize.y = mapSize_.height;
		}

		m_MainCamera.SetWorldBound({{ blockSize, blockSize }, glm::vec2{ mapSize.x, mapSize.y } - glm::vec2{ blockSize, blockSize } });

		const auto& tilesets = map.getTilesets();

		{
			GDIs.reserve(tilesets.size());
			SpriteAtlases.reserve(tilesets.size());

			for (const auto& tileset : tilesets) {
				auto tileSize = tileset.getTileSize();

				auto image = Cori::Graphics::Image::Create(tileset.getImagePath());
				auto atlas = Cori::Graphics::SpriteAtlas::Create(tileset.getName(), image, glm::ivec2{tileSize.x, tileSize.y});
				if (atlas->GetSuccessStatus()) {
					SpriteAtlases.push_back(atlas);
					SpriteAtlasMap.insert({ tileset.getName(), atlas });
					GDIs.emplace_back(tileset.getFirstGID(), tileset.getLastGID());
				} else {
					CORI_ERROR_TAGGED({ "Level Loader" }, "Failed to load a tileset from a tmx file. Tmx path: '{}', Tileset path: '{}'", path.string(), tileset.getImagePath());
				}
			}
		}

		for (const auto& layer : map.getLayers()) {
			if (layer->getType() == tmx::Layer::Type::Tile) {
				const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();

				if (tileLayer.getName() == "BaseLayer") {
					const auto& tiles = tileLayer.getTiles();

					int height = layer->getSize().y;
					int width = layer->getSize().x;

					for (int i = 0; i < height; ++i) {
						for (int j = 0; j < width; ++j) {
							if (tiles[width * i + j].ID != 0) {
								int tilesetID;
								uint32_t tileID = tiles[width * i + j].ID;

								auto comparator = [](const std::pair<uint32_t, uint32_t>& range, uint32_t value) {
									return range.second < value;
								};

								auto it = std::lower_bound(GDIs.begin(), GDIs.end(), tileID, comparator);

								if (it == GDIs.end()) {
									CORI_WARN("LevelLoader: Tile ID is greater than all GDI ranges.");
									tilesetID = 0;
								}
								else if (tileID >= it->first) {
									tilesetID = static_cast<int>(std::distance(GDIs.begin(), it));
								}
								else {
									CORI_WARN("LevelLoader: Tile ID is between some GDI range.");
									tilesetID = 0;
								}

								static uint32_t count = 1;

								auto tile = ActiveScene.CreateEntity("Tile " + std::to_string(count), Tags::StaticTile);
								tile.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{ blockSize / 2.0f, blockSize / 2.0f }, SpriteAtlases.at(tilesetID)->GetTexture(), SpriteAtlases.at(tilesetID)->GetSpriteUVsAtIndex(tileID - GDIs.at(tilesetID).first));
								auto& transform = tile.GetComponents<Cori::World::Components::Entity::Transform>();
								//transform.SetLocalPosition(glm::vec2{ j * blockSize + blockSize / 2.0f, ((height - i) * blockSize) - blockSize / 2.0f });
								transform.SetLocalPosition(GridPosToPixels({ j, i }, { width, height }, true));
								transform.SetLocalDepth(1);

								count++;
							}
						}
					}
				}
				else if (tileLayer.getName() == "TrapLayer") {
					const auto& tiles = tileLayer.getTiles();

					int height = layer->getSize().y;
					int width = layer->getSize().x;

					for (int i = 0; i < height; ++i) {
						for (int j = 0; j < width; ++j) {
							if (tiles[width * i + j].ID != 0) {
								int tilesetID;
								uint32_t tileID = tiles[width * i + j].ID;

								auto comparator = [](const std::pair<uint32_t, uint32_t>& range, uint32_t value) {
									return range.second < value;
								};

								auto it = std::lower_bound(GDIs.begin(), GDIs.end(), tileID, comparator);

								if (it == GDIs.end()) {
									CORI_WARN("LevelLoader: Tile ID is greater than all GDI ranges.");
									tilesetID = 0;
								}
								else if (tileID >= it->first) {
									tilesetID = static_cast<int>(std::distance(GDIs.begin(), it));
								}
								else {
									CORI_WARN("LevelLoader: Tile ID is between some GDI range.");
									tilesetID = 0;
								}

								static uint32_t count = 1;

								auto tile = tilesets[tilesetID].getTile(tileID);

								const auto& properties = tile->properties;
								float damage = 0.0f;
								uint32_t invisibilityTicks = 0;
								uint32_t hitStunTicksPenalty{ 0 };

								Cori::Physics::Vec2 hitImpulse{ 0.0f, 0.0f };
								Cori::Physics::Vec2 hitPlayerVelocityModifier{ 1.0f, 1.0f };


								for (const auto& property : properties) {
									if (property.getName() == "Damage") {
										damage = property.getFloatValue();
									}
									else if (property.getName() == "InvisibilityTicksBonus") {
										invisibilityTicks = property.getIntValue();
									}
									else if (property.getName() == "HitImpulseX") {
										hitImpulse.x = property.getFloatValue();
									}
									else if (property.getName() == "HitImpulseY") {
										hitImpulse.y = property.getFloatValue();
									}
									else if (property.getName() == "InvisibilityTicksBonus") {
										hitStunTicksPenalty = property.getIntValue();
									}
									else if (property.getName() == "HitPlayerVelocityModifierX") {
										hitPlayerVelocityModifier.x = property.getFloatValue();
									}
									else if (property.getName() == "HitPlayerVelocityModifierY") {
										hitPlayerVelocityModifier.y = property.getFloatValue();
									}
								}

								if (damage != 0.0f && invisibilityTicks != 0) {
									auto objects = tile->objectGroup.getObjects();
									if (!objects.empty()) {
										const auto& object = objects[0];
										if (object.getShape() == tmx::Object::Shape::Polygon) {
											auto localPos = glm::vec2(object.getPosition().x, tile->imageSize.y - object.getPosition().y);
											auto points = object.getPoints();

											Cori::Physics::WindingOrder windingOrder = Cori::Physics::GetPolygonWindingOrder(points);
											if (CORI_CHECK(windingOrder != Cori::Physics::WindingOrder::COLLINEAR, "Polygon is collinear. Cannot create chain collider for object with UID: {}", object.getUID())) { continue; }

											std::vector<Cori::Physics::Vec2> b2points;
											b2points.reserve(points.size());

											if (windingOrder == Cori::Physics::WindingOrder::CLOCKWISE) {
												b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ points.at(0).x, -points.at(0).y }));

												for (int i = points.size() - 1; i > 0; --i) {
													b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ points.at(i).x, -points.at(i).y }));
												}
											}
											else if (windingOrder == Cori::Physics::WindingOrder::COUNTER_CLOCKWISE) {
												for (tmx::Vector2f p : points) {
													b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ p.x, -p.y }));
												}
											}

											auto trap = ActiveScene.CreateEntity("Chain Collider " + std::to_string(count), Tags::ChainCollider);

											Cori::Physics::Body::Params bp;
											bp.type = b2_staticBody;
											bp.position = Cori::Physics::ToMeters(GridPosToPixels({ j, i }, { width, height }, false) + localPos);
											bp.name = "Trap";

											auto& rb = trap.AddComponent<Cori::World::Components::Entity::RigidBody>(ActiveScene.GetPhysicsWorld(), bp, trap);

											Cori::Physics::Chain::Params cp;
											cp.count = b2points.size();
											cp.points = b2points.data();
											cp.isLoop = true;
											cp.filter.categoryBits = Cori::Physics::CollisionBits::StaticBit;

											rb.CreateChain(Cori::Physics::DestroyWithParent, cp);

											Cori::Physics::Shape::Params spa;
											spa.filter.categoryBits = Cori::Physics::CollisionBits::SensorBit;
											spa.isSensor = true;
											spa.enableSensorEvents = true;

											rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Polygon::CreatePolygon(Cori::Physics::ConvexHull::Create(b2points)));

											auto& trig = trap.AddComponent<Cori::World::Components::Entity::Trigger>(trap);
											auto* behavior = trig.SetBehavior<Triggers::SpikeTrap>();
											if (behavior) {
												behavior->m_Damage = damage;
												behavior->m_InvisibilityTicksBonus = invisibilityTicks;
												behavior->m_HitImpulse = hitImpulse;
												behavior->m_HitStunTicksPenalty = hitStunTicksPenalty;
												behavior->m_HitPlayerVelocityModifier = hitPlayerVelocityModifier;
											}

											auto& trapTr = trap.GetComponents<Cori::World::Components::Entity::Transform>();
											auto tilePos = GridPosToPixels({ j, i }, { width, height }, true);

											trapTr.SetLocalPosition(tilePos);
											trapTr.SetLocalDepth(1);

											trap.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2{ blockSize / 2.0f, blockSize / 2.0f }, SpriteAtlases.at(tilesetID)->GetTexture(), SpriteAtlases.at(tilesetID)->GetSpriteUVsAtIndex(tileID - GDIs.at(tilesetID).first));
											count++;
										}
									}
								}
							}
						}
					}
				}
			}
			else if (layer->getType() == tmx::Layer::Type::Object) {
				const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

				if (objectLayer.getName() == "Colliders") {
					const auto& objects = objectLayer.getObjects();

					for (const auto& object : objects) {
						if (object.getShape() == tmx::Object::Shape::Polygon) {
							auto pos = object.getPosition();
							auto points = object.getPoints();

							Cori::Physics::WindingOrder windingOrder = Cori::Physics::GetPolygonWindingOrder(points);
							if (CORI_CHECK(windingOrder != Cori::Physics::WindingOrder::COLLINEAR, "Polygon is collinear. Cannot create chain collider for object with UID: {}", object.getUID())) { continue; }

							std::vector<Cori::Physics::Vec2> b2points;
							b2points.reserve(points.size());

							if (windingOrder == Cori::Physics::WindingOrder::CLOCKWISE) {
								b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ points.at(0).x, -points.at(0).y }));

								for (int i = points.size() - 1; i > 0; --i) {
									b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ points.at(i).x, -points.at(i).y }));
								}
							}
							else if (windingOrder == Cori::Physics::WindingOrder::COUNTER_CLOCKWISE) {
								for (tmx::Vector2f p : points) {
									b2points.push_back(Cori::Physics::ToMeters(glm::vec2{ p.x, -p.y }));
								}
							}

							static uint32_t count = 1;

							auto col = ActiveScene.CreateEntity("Chain Collider " + std::to_string(count), Tags::ChainCollider);

							Cori::Physics::Body::Params bp;
							bp.type = b2_staticBody;
							bp.position = Cori::Physics::ToMeters(TiledPosToPixels(pos, { mapSize.x, mapSize.y }));

							auto& rb = col.AddComponent<Cori::World::Components::Entity::RigidBody>(ActiveScene.GetPhysicsWorld(), bp, col);

							Cori::Physics::Chain::Params cp;
							cp.count = b2points.size();
							cp.points = b2points.data();
							cp.isLoop = true;
							cp.filter.categoryBits = Cori::Physics::CollisionBits::StaticBit;

							rb.CreateChain(Cori::Physics::DestroyWithParent, cp);
							count++;
						}
					}
				}
				else if (objectLayer.getName() == "Points") {
					const auto& objects = objectLayer.getObjects();

					for (const auto& object : objects) {
						if (object.getName() == "ActorSpawn" && object.getShape() == tmx::Object::Shape::Point) {
							if (!playerSpawnFound) {
								const auto pos = object.getPosition();

								CreatePlayer(initialPlayerTime, TiledPosToPixels(pos, { mapSize.x, mapSize.y }));
								playerSpawnFound = true;
							} else {
								CORI_ERROR("Found at least 2 actor spawns.");
							}
						}
						else if (object.getName() == "EscapePortal" && object.getShape() == tmx::Object::Shape::Point) {
							const auto pos = object.getPosition();

							CreateEscapeDoor(Cori::Physics::ToMeters(TiledPosToPixels(pos, { mapSize.x, mapSize.y })));

							escapeFound = true;
						}
					}
				}
				else if (objectLayer.getName() == "RegularOrbs") {
					const auto& objects = objectLayer.getObjects();

					for (const auto& object : objects) {
						if (object.getShape() == tmx::Object::Shape::Point) {
							const auto pos = object.getPosition();

							AddRegularOrb(orbBonus, Cori::Physics::ToMeters(TiledPosToPixels(pos, { mapSize.x, mapSize.y })));
						}
					}
				}
				else if (objectLayer.getName() == "MovingPlatforms") {
					auto system = ActiveScene.GetSystem<Systems::MovingPlatform>();
					if (system) {
						auto locked = system->lock();
						const auto& objects = objectLayer.getObjects();

						for (const auto& object : objects) {
							if (object.getShape() == tmx::Object::Shape::Point) {
								const auto pos = Cori::Physics::ToMeters(TiledPosToPixels(object.getPosition(), { mapSize.x, mapSize.y }));

								PlatformParams pp;
								pp.expressionX.RegisterValues({ "t" });
								pp.expressionY.RegisterValues({ "t" });

								std::string exprX;
								std::string exprY;

								bool success = true;
								bool textureFound = false;

								const auto properties = object.getProperties();
								for (const auto& property : properties) {
									if (property.getName() == "AtlasName") {
										if (SpriteAtlasMap.contains(property.getStringValue())) {
											pp.atlas = SpriteAtlasMap[property.getStringValue()];
										} else {
											CORI_ERROR("Failed to find atlas by name '{}' for assigning to the moving platform, platform will not be created.", property.getStringValue());
											success = false;
										}
									} else if (property.getName() == "BodyTileID") {
										pp.bodyTileID = property.getIntValue();
									}
									else if (property.getName() == "LeftOrBottomCornerTileID") {
										pp.leftOrBottomCornerTileID = property.getIntValue();
									}
									else if (property.getName() == "RightOrTopCornerTileID") {
										pp.rightOrTopCornerTileID = property.getIntValue();
									}
									else if (property.getName() == "Size") {
										pp.size = property.getIntValue();
									}
									else if (property.getName() == "Depth") {
										pp.m_Depth = property.getIntValue();
									}
									else if (property.getName() == "IsVertical") {
										pp.vertical = property.getBoolValue();
									}
									else if (property.getName() == "ExpressionX") {
										exprX = property.getStringValue();
									}
									else if (property.getName() == "ExpressionY") {
										exprY = property.getStringValue();
									}
									else {
										if (property.getType() == tmx::Property::Type::String) {
											pp.expressionX.AddAlias(property.getName(), property.getStringValue());
											pp.expressionY.AddAlias(property.getName(), property.getStringValue());
										}
									}
								}


								pp.expressionX.Parse(exprX);
								pp.expressionY.Parse(exprY);

								if (pp.expressionY.Success() && pp.expressionX.Success() && success) {
									pp.positionOffset = pos;
									locked->CreatePlatform(pp);
								} else {
									CORI_ERROR("Failed to create moving platform");
								}


							}
						}
					}
				}
			}
		}

		CORI_ASSERT(playerSpawnFound, "No player spawn found in level: {}", path.string());

		CORI_CHECK(escapeFound, "No escape door was found in level: {}", path.string());

		m_LevelLoaded = true;
	}
}

void LevelLayer::AddRegularOrb(const float orbBonus, const Cori::Physics::Vec2 pos) {
	auto tr = ActiveScene.CreateEntity("Regular Orb", Tags::Triggers);

	Cori::Physics::Body::Params bp;
	bp.type = b2_staticBody;
	bp.position = pos;
	bp.name = "Regular Orb";

	auto& rb = tr.AddComponent<Cori::World::Components::Entity::RigidBody>(ActiveScene.GetPhysicsWorld(), bp, tr);

	Cori::Physics::Shape::Params spa;
	spa.filter.categoryBits = Cori::Physics::CollisionBits::SensorBit;
	spa.isSensor = true;
	spa.enableSensorEvents = true;

	constexpr float orbRadius = 0.2f;

	rb.CreateShape(Cori::Physics::DestroyWithParent, spa, Cori::Physics::Circle::Create({ orbRadius / 2.0f, orbRadius / 2.0f }, orbRadius));

	auto& trig = tr.AddComponent<Cori::World::Components::Entity::Trigger>(tr);
	trig.SetBehavior<Triggers::RegularOrb>();
	const auto behavior = trig.GetBehavior<Triggers::RegularOrb>();
	if (behavior) {
		behavior.value()->m_TimeBonus = orbBonus;
	}

	const auto atlas = Cori::AssetManager::Get(Assets::Coin);
	auto& trtr = tr.GetComponents<Cori::World::Components::Entity::Transform>();
	trtr.SetLocalPosition(Cori::Physics::ToPixels(bp.position + Cori::Physics::Vec2(orbRadius / 2.0f, orbRadius / 2.0f)));
	trtr.SetLocalDepth(3);

	tr.AddComponent<Cori::World::Components::Entity::QuadRenderer>(glm::vec2(16, 16), atlas->GetTexture(), atlas->GetSpriteUVsAtIndex(0));
}

