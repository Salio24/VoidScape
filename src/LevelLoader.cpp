#include "LevelLoader.hpp"

void LevelLoader::LoadLevel(std::shared_ptr<Cori::Scene> scene, const std::string& path) {
	// temp vvv
	int blockSize = CORI_PIXELS_PER_METER;

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

				SpriteAtlases.push_back(Cori::AssetManager::GetSpriteAtlasOwning(atlas));
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
								tile.AddComponent<Cori::Components::Entity::Render>(glm::vec2{ j * blockSize, ((height - i) * blockSize) - blockSize }, glm::vec2{ blockSize, blockSize });
								tile.AddComponent<Cori::Components::Entity::Sprite>(SpriteAtlases.at(tilesetID)->GetTexture(), SpriteAtlases.at(tilesetID)->GetSpriteUVsAtIndex(tileID - GDIs.at(tilesetID).first));
							}
						}
					}
					scene->SortRenderGroup();
				}
			}
			else if (layer->getType() == tmx::Layer::Type::Object) {
				const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

				if (objectLayer.getName() == "Colliders") {
					const auto& objects = objectLayer.getObjects();
					int height = mapSize.height;
					int width = mapSize.width;

					for (const auto& object : objects) {
						if (object.getShape() == tmx::Object::Shape::Polygon) {
							auto pos = object.getPosition();
							auto points = object.getPoints();

							Cori::Physics::WindingOrder windingOrder = Cori::Physics::GetPolygonWindingOrder(points);
							if (CORI_ASSERT_WARN(windingOrder != Cori::Physics::WindingOrder::COLLINEAR, "Polygon is collinear. Cannot create chain collider for object with UID: {}", object.getUID())) { continue; }

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

							auto col = scene->CreateEntity();

							Cori::Physics::Body::Params bp;
							bp.type = b2_staticBody;
							bp.position = Cori::Physics::ToMeters(glm::vec2{ pos.x, ((height - pos.y))});

							auto& rb = col.AddComponent<Cori::Components::Entity::Rigidbody>(scene->PhysicsWorld, bp);

							Cori::Physics::Chain::Params cp;
							cp.count = b2points.size();
							cp.points = b2points.data();
							cp.isLoop = true;

							rb.CreateChain(Cori::Physics::DestroyWithParent, cp);
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


