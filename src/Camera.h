#pragma once

#include "Vec2.h"
#include "CameraTypes.h"

class Camera
{
public:
	virtual ~Camera() = default;
	virtual void onEnter(const CameraContext& ctx) = 0;
	virtual CameraEvents update(const CameraContext& ctx) = 0;
	virtual Vec2 center() const = 0;
};