#include "LevelLoader.hpp"

void LevelLoader::LoadLevel(std::shared_ptr<Cori::Scene> scene, const std::string& path) {
	// temp vvv
	int blockSize = 16;

	tmx::Map map;

	std::vector<std::pair<uint32_t, uint32_t>> GDIs;

	std::vector<std::shared_ptr<Cori::SpriteAtlas>> SpriteAtlases;

	if (map.load(path)) {

		auto gridSize = map.getTileSize();

		const auto mapSize = map.getBounds();

		{
			const auto tilesets = map.getTilesets();

			GDIs.reserve(tilesets.size());
			SpriteAtlases.reserve(tilesets.size());

			for (const auto& tileset : tilesets) {
				auto tileSize = tileset.getTileSize();
				std::string atlasName = tileset.getName();
				std::string textureName = atlasName + " Texture";

				const Cori::Texture2DDescriptor texture{
					textureName,
					tileset.getImagePath()
				};

				const Cori::SpriteAtlasDescriptor atlas{
					atlasName,
					texture,
					{tileSize.x, tileSize.y}
				};

				SpriteAtlases.push_back(Cori::AssetManager::GetSpriteAtlas(atlas));
				GDIs.push_back(std::make_pair(tileset.getFirstGID(), tileset.getLastGID()));

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
									CORI_ASSERT_WARN(false, "Tile ID is greater than all GDI ranges.");
									tilesetID = 0;
								}
								else if (tileID >= it->first) {
									tilesetID = std::distance(GDIs.begin(), it);
								}
								else {
									CORI_ASSERT_WARN(false, "Tile ID is between some GDI range.");
									tilesetID = 0;
								}

								auto tile = scene->CreateEntity();
								tile.AddComponent<Cori::RenderingComponent>(glm::vec2{ blockSize, blockSize });
								tile.AddComponent<Cori::PositionComponent>(glm::vec2{ j * blockSize, ((height - i) * blockSize) - blockSize });
								tile.AddComponent<Cori::SpriteComponent>(SpriteAtlases.at(tilesetID)->GetTexture(), SpriteAtlases.at(tilesetID)->GetSpriteUVsAtIndex(tileID - GDIs.at(tilesetID).first));
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

						auto aabb = object.getAABB();

						auto collider = scene->CreateEntity();
						collider.AddComponent<Cori::PositionComponent>(glm::vec2{ ((aabb.left) / gridSize.x) * blockSize, ((mapSize.height - (aabb.top + aabb.height)) / gridSize.y) * blockSize });
						collider.AddComponent<Cori::ColliderComponent>(glm::vec2{ (aabb.width / gridSize.x) * blockSize, (aabb.height / gridSize.y) * blockSize });


						const auto& props = object.getProperties();

						for (const auto& prop : props) {
							if (prop.getName() == "death_trigger") {
								collider.AddComponent<Cori::TriggerComponent>([](Cori::Entity& trigger, Cori::Entity& entity, Cori::EventCallbackFn eventCallback) -> bool {

									// set player health to 0



									return true;
								});
							}
						}
					}
				}
				else if (objectLayer.getName() == "Points") {
					const auto& objects = objectLayer.getObjects();
				
					for (const auto& object : objects) {
						if (object.getName() == "ActorSpawn" && object.getShape() == tmx::Object::Shape::Point) {
							const auto pos = object.getPosition();
				
							// set actor spawn
						}
						else if (object.getName() == "EscapePortal" && object.getShape() == tmx::Object::Shape::Point) {
							const auto pos = object.getPosition();

							// set escape portal spawn
						}
					}
				}
			}
		}
	}
}


