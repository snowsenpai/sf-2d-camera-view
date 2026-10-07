#pragma once

#include "Vec2.h"
#include "CameraTypes.h"
#include "Camera.h"

class FollowCamera : public Camera
{
	Vec2 m_center;
	Vec2 computeCenter(const CameraContext& ctx);

	// One axis: the camera-center value that keeps the view inside the world.
	float clampAxis(float desired, float viewSize, float worldSize);
public:
	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;
};