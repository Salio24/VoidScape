#include <iostream>
#include <Cori.hpp>
#include <imgui.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class ExampleLayer : public Cori::Layer {
public:
	ExampleLayer() : Layer("Example") {
		//Cori::Application::GetWindow().SetVSync(true);

		m_Camera.SetCameraSize(0, 7680, 0, 4320);

	}

	virtual void OnEvent(Cori::Event& event) override {
		if (!event.IsOfType(Cori::EventType::MouseMoved)) {
			CORI_TRACE("| Layer: {0} | Event: {1}", this->GetName(), event);
		}
	}

	virtual void OnImGuiRender(const double deltaTime) override {
		ImGui::Begin("Test");
		ImGui::SliderFloat("Test float", &testFloat, 0.0f, 150.0f);
		ImGui::End();
	}

	void OnUpdate(const double deltaTime) override {

		// TODO, get rid of raw pointers in create funcs

		Cori::GraphicsCall::SetViewport(0, 0, Cori::Application::GetWindow().GetWidth(), Cori::Application::GetWindow().GetHeight());
		Cori::GraphicsCall::SetClearColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		Cori::GraphicsCall::ClearFramebuffer();

		Cori::Renderer2D::BeginBatch(m_Camera.GetViewProjectionMatrix(), model);

		Cori::Renderer2D::DrawQuad(glm::vec2(50.0f, 50.0f + testFloat), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		Cori::Renderer2D::DrawQuad(glm::vec2(150.0f, 50.0f + testFloat), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		Cori::Renderer2D::DrawQuad(glm::vec2(250.0f, 50.0f + testFloat), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		Cori::Renderer2D::DrawQuad(glm::vec2(350.0f, 50.0f + testFloat), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		Cori::Renderer2D::DrawQuad(glm::vec2(450.0f, 50.0f + testFloat), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		Cori::Renderer2D::EndBatch();
	}

	virtual void OnTickUpdate() override {

	}

	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 0.0f));

	Cori::OrthoCamera m_Camera;

	float testFloat = 0.0f;

};

class Sandbox : public Cori::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());

		CORI_INFO("Sandbox application created");
	}

	~Sandbox() {
		CORI_INFO("Sandbox application destroyed");
	}
};

Cori::Application* Cori::CreateApplication() {
	return new Sandbox();
}