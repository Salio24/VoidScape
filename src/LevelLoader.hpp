#pragma once
#include <Cori.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>

class LevelLoader {
public:
	static void LoadLevel(std::shared_ptr<Cori::Scene> scene, const std::string& path);
};