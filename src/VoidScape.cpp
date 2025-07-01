#define CORI_PIXELS_PER_METER 16
#define CORI_MAX_TRIGGER_VISITORS 2
#include <Cori.hpp>
#include <CoriEntry.hpp>
#include "GameLayer.hpp"
#include <print>

void Lol() {
	std::println("Ne proshlo i veka LOL");
}

class VoidScape : public Cori::Application {
public:
	VoidScape() {
		PushLayer(new GameLayer());

		CORI_INFO("VoidScape application created");
	}

	~VoidScape() {
		CORI_INFO("VoidScape application destroyed");
	}
};

Cori::Application* Cori::CreateApplication() {
	return new VoidScape();
}