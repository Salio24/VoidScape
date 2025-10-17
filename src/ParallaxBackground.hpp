#pragma once
#include <Cori.hpp>

class ParallaxBackground {
public:
	ParallaxBackground() = default;

	void SetCameraBounds(const glm::vec2 bounds) {
		m_CameraBounds = bounds;
	}

	void AddLayer(const std::string& name, const std::filesystem::path& topImagePath, const std::filesystem::path& mainImagePath, const std::filesystem::path& bottomImagePath, const glm::vec2 speed, const glm::vec2 offset, const uint16_t screenHeight, const uint8_t depth) {
		auto createTexture = [](const std::filesystem::path& path) {
			if (path.empty()) return std::shared_ptr<Cori::Graphics::Texture2D>(nullptr);
			const auto image = Cori::Graphics::Image::Create(path);
			return Cori::Graphics::Texture2D::Create(image);
		};

		LayerTextures textures{
			createTexture(topImagePath),
			createTexture(mainImagePath),
			createTexture(bottomImagePath)
		};

		LayerParams params{speed, offset, screenHeight, depth, true, false};
		m_Layers.emplace_back(Layer{name, textures, params});
	}

	void AddLayer(const std::string& name, const std::filesystem::path& mainImagePath, const glm::vec2 speed, const glm::vec2 offset, const uint16_t screenHeight, const uint8_t depth, const bool clampToEdge)
	{
		auto createTexture = [](const std::filesystem::path& path) {
			if (path.empty()) return std::shared_ptr<Cori::Graphics::Texture2D>(nullptr);
			const auto image = Cori::Graphics::Image::Create(path);
			return Cori::Graphics::Texture2D::Create(image);
		};

		LayerTextures textures{
			nullptr,
			createTexture(mainImagePath),
			nullptr
		};

		LayerParams params{speed, offset, screenHeight, depth, false, clampToEdge};
		m_Layers.emplace_back(Layer{name, textures, params});
	}

	void Render(glm::vec2 position, glm::vec2 offset) const {
		CORI_PROFILE_FUNCTION();

		const glm::vec2 cameraBottomLeft = { position.x - m_CameraBounds.x * 0.5f, position.y };
		const glm::vec2 cameraTopRight   = { position.x + m_CameraBounds.x * 0.5f, position.y + m_CameraBounds.y };
		//offset = m_offset;

		for (const auto& layer : m_Layers) {
			const auto& mainTexture = layer.m_Textures.m_MainTexture;
			const auto& params = layer.m_Params;
			const glm::vec2 layerPosition = position;
			const glm::vec2 layerOffset = offset + params.m_Offset;

			if (!mainTexture || mainTexture->GetWidth() == 0 || mainTexture->GetHeight() == 0 || params.m_ScreenHeight == 0 || m_CameraBounds.y <= 0.0f) {
				continue;
			}

			const float scale = m_CameraBounds.y / static_cast<float>(params.m_ScreenHeight);
			const float quadHeight = static_cast<float>(mainTexture->GetHeight()) * scale;
			const float quadWidth = static_cast<float>(mainTexture->GetWidth()) * scale;
			const glm::vec2 halfQuadSize = { quadWidth * 0.5f, quadHeight * 0.5f};

			const float parallaxDisplacementX = - layerPosition.x * params.m_Speed.x;
			const float firstTileCenterX = parallaxDisplacementX + floor((cameraBottomLeft.x - parallaxDisplacementX) / quadWidth) * quadWidth + halfQuadSize.x + layerOffset.x;

			auto drawRow = [&](const std::shared_ptr<Cori::Graphics::Texture2D>& texture, const float y) {
				if (!texture) return;

				Cori::Graphics::Renderer2D::ObjectTransparency layerTransparency;
				if (texture->HasSemiTransparency()) {
					layerTransparency = Cori::Graphics::Renderer2D::ObjectTransparency::SEMI_TRANSPARENT;
				}
				else {
					layerTransparency = Cori::Graphics::Renderer2D::ObjectTransparency::OPAQUE;
				}

				for (float x = firstTileCenterX; x - halfQuadSize.x < cameraTopRight.x; x += quadWidth) {
					const glm::mat3 transform = glm::translate(glm::mat3(1.0f), { x, y });
					constexpr Cori::Graphics::UVs uvs = {{ 0.0f, 0.0f }, { 1.0f, 1.0f }};
					Cori::Graphics::Renderer2D::SubmitQuad(Cori::Graphics::Renderer2D::DrawSpace::WORLD_SPACE, layerTransparency, transform, halfQuadSize, glm::vec4(1.0f), texture.get(), uvs, params.m_Depth, false, false, false);
				}
			};

			if (params.m_InfiniteYScroll) {
				const float parallaxDisplacementY = layerOffset.y - layerPosition.y * params.m_Speed.y;
				const int32_t startTileIndexY = floor((cameraBottomLeft.y - parallaxDisplacementY) / quadHeight);

				for (int32_t yIndex = startTileIndexY; ; ++yIndex) {
					const float currentTileCenterY = parallaxDisplacementY + static_cast<float>(yIndex) * quadHeight + halfQuadSize.y;

					if ((currentTileCenterY - halfQuadSize.y * 2.0f) > cameraTopRight.y) {
						break;
					}

					if (yIndex == 0) {
						drawRow(layer.m_Textures.m_MainTexture, currentTileCenterY);
					}
					else if (yIndex > 0) {
						drawRow(layer.m_Textures.m_TopTexture, currentTileCenterY);
					}
					else {
						drawRow(layer.m_Textures.m_BottomTexture, currentTileCenterY);
					}
				}
			}
			else {

				const float parallaxDisplacementY = - layerPosition.y * params.m_Speed.y;
				const float currentTileCenterY = parallaxDisplacementY + halfQuadSize.y + layerOffset.y;

				const float highestAllowedCenterY = cameraBottomLeft.y + halfQuadSize.y;
				const float lowestAllowedCenterY  = cameraTopRight.y - halfQuadSize.y;

				float finalCenterY = currentTileCenterY;
				if (params.m_NonInfiniteYClampToEdge && highestAllowedCenterY >= lowestAllowedCenterY) {
					finalCenterY = std::clamp(currentTileCenterY, lowestAllowedCenterY, highestAllowedCenterY);
				}

				drawRow(mainTexture, finalCenterY);
			}
		}
	}

	void ImGui() {
		ImGui::Begin("Parallax Background");

		ImGui::SliderFloat("offset x", &m_Offset.x, -500.0f, 500.f);

		ImGui::SliderFloat("offset y", &m_Offset.y, -500.0f, 500.f);

		for (auto& layer : m_Layers) {
			ImGui::SeparatorText(layer.m_Name.c_str());

			ImGui::PushID(layer.m_Name.c_str());

			ImGui::SliderFloat("SpeedX", &layer.m_Params.m_Speed.x, -2.0f, 2.0f);
			ImGui::SliderFloat("SpeedY", &layer.m_Params.m_Speed.y, -2.0f, 2.0f);

			ImGui::SliderFloat("OffsetX", &layer.m_Params.m_Offset.x, -500.0f, 500.0f);
			ImGui::SliderFloat("OffsetY", &layer.m_Params.m_Offset.y, -500.0f, 500.0f);

			ImGui::Checkbox("Clamp", &layer.m_Params.m_NonInfiniteYClampToEdge);

			ImGui::PopID();

			ImGui::Separator();
		}

		ImGui::End();
	}

private:
	struct LayerParams {
		glm::vec2 m_Speed;
		glm::vec2 m_Offset;
		uint16_t m_ScreenHeight;
		uint8_t m_Depth;
		bool m_InfiniteYScroll;
		bool m_NonInfiniteYClampToEdge;
	};

	struct LayerTextures {
		std::shared_ptr<Cori::Graphics::Texture2D> m_TopTexture;
		std::shared_ptr<Cori::Graphics::Texture2D> m_MainTexture;
		std::shared_ptr<Cori::Graphics::Texture2D> m_BottomTexture;
	};

	struct Layer {
		LayerTextures m_Textures;
		LayerParams m_Params;
		std::string m_Name;
		Layer() = default;
		Layer(const std::string& name, const LayerTextures& textures, const LayerParams& params) : m_Textures(textures), m_Params(params), m_Name(name) {}
	};

	glm::vec2 m_Offset{ 0.0f, 0.0f };

	std::vector<Layer> m_Layers;
	glm::vec2 m_CameraBounds{ 0.0f, 0.0f };
};
