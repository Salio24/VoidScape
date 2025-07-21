#define CORI_PIXELS_PER_METER 16
#define CORI_MAX_TRIGGER_VISITORS 2
#define CORI_ASYNC_LOGGING
#define CORI_CHECK_TAG_COLLISION
#include <Cori.hpp>
#include <CoriEntry.hpp>
#include "GameLayer.hpp"


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
