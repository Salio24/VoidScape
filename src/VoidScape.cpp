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
