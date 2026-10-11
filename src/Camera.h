#pragma once

#include "Vec2.h"
#include "CameraTypes.h"

#include <SFML/Graphics/RenderTarget.hpp>

class Camera
{
public:
	virtual ~Camera() = default;
	virtual void onEnter(const CameraContext& ctx) = 0;
	virtual CameraEvents update(const CameraContext& ctx) = 0;
	virtual Vec2 center() const = 0;

	virtual float zoom() const { return 1.f; }
	virtual void zoomBy(float) {}
	virtual bool blocksGameplay() const { return false; }
	virtual void debugDraw(sf::RenderTarget& target) const = 0;
};