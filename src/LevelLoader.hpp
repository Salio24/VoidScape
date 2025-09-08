#pragma once
#include <Cori.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include "Tags.hpp"

class LevelLoader {
public:
	static void LoadLevel(Cori::World::SceneHandle& scene, const std::string& path);
};