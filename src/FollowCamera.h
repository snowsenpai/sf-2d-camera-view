#pragma once

#include "Vec2.h"
#include "CameraTypes.h"
#include "Camera.h"

class FollowCamera : public Camera
{
	Vec2 m_center;
public:
	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;
};