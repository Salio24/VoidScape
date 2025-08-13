#pragma once
#include <Cori.hpp>
#include <PerlinNoise.hpp>

class MainCamera {
public:
	MainCamera() {
		m_WorldBound.m_Min.x = -std::numeric_limits<float>::max();
		m_WorldBound.m_Min.y = -std::numeric_limits<float>::max();

		m_WorldBound.m_Max.x = std::numeric_limits<float>::max();
		m_WorldBound.m_Max.y = std::numeric_limits<float>::max();
		m_TransformShakeNoiseX.reseed(1u);
		m_TransformShakeNoiseY.reseed(7u);
		m_RotationalShakeNoise.reseed(13u);
	}

	~MainCamera() = default;

	void OnUpdate(const Cori::GameTimer& gameTimer, Cori::CameraController& actualCamera) {

		if (m_GetPosOneshot) {
			m_CameraPosition = actualCamera.GetPosition();
			m_OldCameraPosition = actualCamera.GetPosition();
			m_GetPosOneshot = false;
		}

		glm::vec2 interpolatedPos = { m_CameraPosition * static_cast<float>(gameTimer.GetTickAlpha()) + m_OldCameraPosition * (1.0f - static_cast<float>(gameTimer.GetTickAlpha())) };

		//float clampedTrauma = std::clamp(m_Trauma, 0.0f, 1.0f);
		float shake = m_Trauma * m_Trauma;

		interpolatedPos.x += m_MaxTransformShakeX * shake * m_TransformShakeNoiseX.noise1D(gameTimer.GetMilliseconds() * m_TransformShakeFrequencyModifier);
		interpolatedPos.y += m_MaxTransformShakeY * shake * m_TransformShakeNoiseY.noise1D(gameTimer.GetMilliseconds() * m_TransformShakeFrequencyModifier);

		float rotation = 0.0f;

		rotation += m_MaxRotationalShake * shake * m_RotationalShakeNoise.noise1D(gameTimer.GetMilliseconds() * m_RotationalShakeFrequencyModifier);

		actualCamera.SetRotation(rotation);
		actualCamera.SetPosition(interpolatedPos);
		actualCamera.RecalculateVP();
	}

	void OnTickUpdate(const float timeStep, const glm::vec2 playerPos, const glm::vec2 playerHalfSize, const glm::vec2 playerVelocity, Cori::CameraController& actualCamera) {
		m_OldCameraPosition = m_CameraPosition;

		//m_CameraPosition = playerPos - glm::vec2{320.0f, 180.0f};

		glm::vec2 targetRaw = playerPos - glm::vec2{m_RenderCameraSize.x / 2.0f - playerVelocity.x * m_VelocityDependencyModifierX, m_RenderCameraSize.y / 2.0f - playerVelocity.y * m_VelocityDependencyModifierY};

		glm::vec2 target;

		target.x = std::clamp(targetRaw.x, m_WorldBound.m_Min.x + m_RenderCameraSize.x / 2.0f, m_WorldBound.m_Max.x - m_RenderCameraSize.x / 2.0f);
		//target.x = targetRaw.x;

		target.y = std::clamp(targetRaw.y, m_WorldBound.m_Min.y + m_RenderCameraSize.y / 2.0f, m_WorldBound.m_Max.y - m_RenderCameraSize.y / 2.0f);
		//target.y = targetRaw.y;

		m_CameraPosition.x += (target.x - m_CameraPosition.x) * m_AsymptoticAverageX;
		m_CameraPosition.y += (target.y - m_CameraPosition.y) * m_AsymptoticAverageY;

		m_Trauma -= 0.01f;

		m_Trauma = std::clamp(m_Trauma, 0.0f, 1.0f);

		//CORI_DEBUG("Trauma: {}", m_Trauma);

	}

	void AddTrauma(float value) {
		float trauma = m_Trauma + value;
		m_Trauma = std::clamp(trauma, 0.0f, 1.0f);
	}

	void SetWorldBound(const Cori::Utility::AABB& worldBound) {
		m_WorldBound = worldBound;
	};

	Cori::Utility::AABB GetWorldBound() const {
		return m_WorldBound;
	}

	void SetRenderCameraSize(const glm::vec2& renderCameraSize) {
		m_RenderCameraSize = renderCameraSize;
	}

	bool m_PlayerJustLanded{ false };

	float m_MaxTransformShakeX{ 350.0f };
	float m_MaxTransformShakeY{ 350.0f };

	float m_AsymptoticAverageX{ 0.08f };
	float m_AsymptoticAverageY{ 0.16f };

	float m_VelocityDependencyModifierX{ 0.40f };
	float m_VelocityDependencyModifierY{ 0.0f };

	float m_MaxRotationalShake{ 30.0f };
	float m_RotationalShakeFrequencyModifier{ 1.0f };

	float m_TransformShakeFrequencyModifier{ 1.0f };

	float m_TraumaOnFall{ 0.1f };

private:
	siv::BasicPerlinNoise<float> m_TransformShakeNoiseX;
	siv::BasicPerlinNoise<float> m_TransformShakeNoiseY;
	siv::BasicPerlinNoise<float> m_RotationalShakeNoise;

	glm::vec2 m_CameraPosition{ 0.0f, 0.0f };
	glm::vec2 m_OldCameraPosition{ 0.0f, 0.0f };

	Cori::Utility::AABB m_WorldBound{};

	glm::vec2 m_RenderCameraSize{ 0.0f, 0.0f };

	bool m_GetPosOneshot{ true };

	float m_Trauma{ 0.0f };

};
