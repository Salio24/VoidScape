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