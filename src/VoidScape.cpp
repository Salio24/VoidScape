#define CORI_PIXELS_PER_METER 16
#define CORI_MAX_TRIGGER_VISITORS 2
//#define CORI_ASYNC_LOGGING
#define CORI_CHECK_TAG_COLLISION
#include <Cori.hpp>
#include <CoriEntry.hpp>
#include "GameLayer.hpp"


class VoidScape : public Cori::Core::Application {
public:
	VoidScape() : Application("VoidScape") {
		PushLayer(new GameLayer());

		SetBackgroundColor(glm::vec4(14.0f / 256.0f, 7.0f / 256.0f, 27.0f / 256.0f, 1.0f));

		CORI_INFO("VoidScape application created");
	}

	~VoidScape() override {
		CORI_INFO("VoidScape application destroyed");
	}
};

Cori::Core::Application* Cori::Core::CreateApplication() {
	return new VoidScape();
}
