#pragma once

#include "Camera.h"
#include "CameraTypes.h"
#include "Vec2.h"

#include <SFML/Graphics/RenderTarget.hpp>

struct FollowCamConfig
{
	Vec2 windowHalfSize{};
	float smoothing=0.f;
};

class FollowCamera : public Camera
{
	FollowCamConfig m_config;

	Vec2 m_center;
	Vec2 computeCenter(const CameraContext& ctx) const;

	// One axis: the camera-center value that keeps the view inside the world.
	static float clampAxis(float desired, float viewSize, float worldSize);

	void debugDraw(sf::RenderTarget& target) const override;
public:
	explicit FollowCamera(const FollowCamConfig& cfg = {})
		: m_config(cfg)
	{};

	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;
};