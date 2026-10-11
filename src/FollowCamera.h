#pragma once

#include "Camera.h"
#include "CameraTypes.h"
#include "Vec2.h"

#include <SFML/Graphics/RenderTarget.hpp>

struct FollowCamConfig
{
	Vec2 windowHalfSize{ 64.f, 48.f }; // world units. Keep it below half the effective view
	float smoothing = 8.f; // 1/seconds. 0 = off (snap)
};

class FollowCamera : public Camera
{
	float m_zoom = 1.f;
	// constants, what are they used for?
	static constexpr float kMinZoom = 0.5f;
	static constexpr float kMaxZoom = 3.f;

	FollowCamConfig m_config;

	Vec2 m_center;
	Vec2 clampToWorld(const Vec2& p, const CameraContext& ctx) const;

	// One axis: the camera-center value that keeps the view inside the world.
	static float clampAxis(float desired, float viewSize, float worldSize);


	static float pushAxis(float center, float target, float halfWindow);

	static float catchUp(float rate, float dt);

	void debugDraw(sf::RenderTarget& target) const override;
public:
	explicit FollowCamera(const FollowCamConfig& cfg = {})
		: m_config(cfg)
	{}

	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;

	float zoom() const override;
	void zoomBy(float factor) override;
};