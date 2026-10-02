#pragma once

#include "Vec2.h"

#include <optional>

struct CameraContext
{
	Vec2 targetPos; // center of what the camera follows (the player)
	Vec2 worldSize;
	Vec2 viewSize;
};

struct EdgeCrossing
{
	float edge; // world coordinate of the shared edge, on the crossing axis
	int	  dir; // -1 or +1, direction of travel
};

struct CameraEvents
{
	std::optional<EdgeCrossing> x; // crossed a vertical boundary (moved along x)
	std::optional<EdgeCrossing> y; // crossed a horizontal boundary (moved along y)
};