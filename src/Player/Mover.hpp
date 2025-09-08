#pragma once
#include <Cori.hpp>
#include "States.hpp"
#include "Components.hpp"

class Mover {
public:
	struct Params {
		float jumpStartSpeed{ 10.0f };
		float jumpVariableSpeed{ 1.0f };
		int   jumpVariableTicks{ 10 };
		int   jumpBufferTicks{ 8 };
		int   jumpCoyoteTimeTicks{ 4 };

		float wallJumpStartSpeed{ 10.0f };
		float wallJumpVariableSpeed{ 1.0f };
		float wallJumpStartSideSpeed{ 10.0f };
		float wallJumpVariableSideSpeed{ 0.0f };
		int   wallJumpVariableTicks{ 10 };
		int   wallJumpBufferTicks{ 4 };

		float doubleJumpStartSpeed{ 10.0f };
		float doubleJumpVariableSpeed{ 1.0f };
		int   doubleJumpVariableTicks{ 10 };
		float doubleJumpRayModifierForRegular{ 2.0f };
		float doubleJumpRayModifierForWall{ 2.0f };

		float maxSpeed{ 6.0f };
		float minSpeed{ 0.1f };
		float stopSpeed{ 3.0f };
		float acceleration{ 20.0f };
		float airSteer{ 0.2f };
		float friction{ 8.0f };
		float gravityDefault{ 30.0f };
		float fastFallGravityModifier{ 1.5f };
		float pogoHertz{ 5.0f };
		float pogoDampingRatio{ 0.8f };
		float pogoLengthScale{ 1.95f };
		float segmentOffset{ 0.65f };
		float wallSlideSpeed{ 1.5f };

		float minSpeedForRunState{ 4.0f };
	};

	Mover(const Cori::Physics::Capsule& capsule, Cori::Physics::WorldRef world, Cori::World::Entity& player, const Params& def);
	~Mover() = default;

	void OnUpdate(const double deltaTime, const double tickAlpha);

	void OnTickUpdate(const float timeStep, MainCamera& mainCamera);

	void UpdateGui();
	void DebugDraw(float test);

	void BindPlayer(const Cori::World::Entity& player) {
		m_Player = player;
	}

	void UnbindPlayer() {
		m_Player = Cori::World::Entity{};
	}

	float m_JumpStartSpeed;
	float m_JumpVariableSpeed;
	int m_JumpVariableTicks;
	int m_JumpBufferTicks;
	int m_JumpCoyoteTimeTicks;

	float m_WallJumpStartSpeed;
	float m_WallJumpVariableSpeed;
	float m_WallJumpStartSideSpeed;
	float m_WallJumpVariableSideSpeed;
	int   m_WallJumpVariableTicks;
	int   m_WallJumpBufferTicks;

	float m_DoubleJumpStartSpeed;
	float m_DoubleJumpVariableSpeed;
	int m_DoubleJumpVariableTicks;
	float m_DoubleJumpRayModifierForRegular;
	// ^^^ this value regulates the length of the ray for raycast that disables double jump near ground, the length is relative to m_DoubleJumpVariableTicks and current m_Velocity.y
	float m_DoubleJumpRayModifierForWall;
	// ^^^ same but for wall jumps
	
	float m_MaxSpeed;
	float m_MinSpeed;
	float m_StopSpeed;
	float m_Acceleration;
	float m_AirSteer;
	float m_Friction;
	float m_GravityDefault;
	float m_FastFallGravityModifier;
	float m_PogoHertz;
	float m_PogoDampingRatio;
	float m_PogoLengthScale;
	float m_SegmentOffset;
	float m_WallSlideSpeed;

	float m_MinSpeedForRunState;

	Cori::Physics::Vec2 m_RenderingPosition;
	Cori::Physics::Vec2 m_OldRenderingPosition;

	Cori::Physics::Vec2 m_Velocity{ 0.0f, 0.0f };

private:
	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult* planeResult, void* context);

	void SolveMove(const float timeStep, float throttle);

	static constexpr int m_PlaneCapacity = 8;

	void SaveSettings(const std::filesystem::path& filepath);
	void LoadSettings(const std::filesystem::path& filepath);

	//Cori::Physics::BodyRef m_SensorVisitorBody;

	Cori::Physics::WorldRef m_World;
	Cori::Physics::Capsule m_Capsule;
	Cori::Physics::Segment m_Segment;

	Cori::Physics::CastResult m_CastResult;
	Cori::Physics::CollisionPlane m_Planes[m_PlaneCapacity] = {};

	Cori::Physics::Transform m_Transform;
	Cori::Physics::Transform m_OldTransform;
	Cori::Physics::Vec2 m_Translation;
	Cori::Physics::Vec2 m_Origin{ 0.0f, 0.0f };
	Cori::Physics::Vec2 m_P1;
	Cori::Physics::Vec2 m_P2;

	Cori::Physics::Vec2 m_GroundRayStart;
	Cori::Physics::Vec2 m_GroundRayEnd;

	Cori::Physics::Vec2 m_WallRayStart;
	Cori::Physics::Vec2 m_WallRayEnd;

	int m_PlaneCount{ 0 };
	int m_TotalIterations{ 0 };
	float m_PogoVelocity{ 0.0f };
	float m_Gravity{ 0.0f };

	bool m_OnGround{ false };
	bool m_OldOnGround{ false };
	bool m_JumpButtonReleased{ true };
	bool m_Jumping{ false };
	bool m_WallJumping{ false };
	bool m_DoubleJumping{ false };

	bool m_ResetDistanceNextTick{ false };

	bool m_CanWallJump{ false };
	bool m_CanDoubleJump{ false };
	int m_WallJumpDirection{ 0 };


	int m_JumpVariableTickTimer{ 128 };
	int m_JumpBufferTickTimer{ 128 };
	int m_JumpCoyoteTimeTickTimer{ 128 };
	
	int m_WallJumpBufferTickTimer{ 128 };
	int m_WallJumpVariableTickTimer{ 128 };

	int m_DoubleJumpVariableTickTimer{ 128 };

	bool m_NearGround{ false };
	bool m_NearWall{ false };

	bool m_PixelAlignedRender{ false };

	float m_LastFallingDistance{ 0.0f };

	float test1{ 1.0f };

	Cori::World::Entity m_Player;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Mover::Params,
	jumpStartSpeed, jumpVariableSpeed, jumpVariableTicks, jumpBufferTicks, jumpCoyoteTimeTicks,
	wallJumpStartSpeed, wallJumpVariableSpeed, wallJumpStartSideSpeed, wallJumpVariableSideSpeed,
	wallJumpVariableTicks, wallJumpBufferTicks,
	doubleJumpStartSpeed, doubleJumpVariableSpeed, doubleJumpVariableTicks,
	doubleJumpRayModifierForRegular, doubleJumpRayModifierForWall,
	maxSpeed, minSpeed, stopSpeed, acceleration, airSteer, friction, gravityDefault,
	fastFallGravityModifier, pogoHertz, pogoDampingRatio, pogoLengthScale, segmentOffset,
	wallSlideSpeed, minSpeedForRunState
)