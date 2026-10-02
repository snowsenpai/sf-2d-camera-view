#pragma once

#include "Vec2.h"
#include "CameraTypes.h"

#include <SFML/System/Vector2.hpp>

class FixedCamera
{
	Vec2 m_cellSize; // one cell == one view
	sf::Vector2i m_gridSize; // cells per axis (cols, rows), derived from world / cell
	sf::Vector2i m_currentCell; // the cell the camera is currently showing

	sf::Vector2i cellFromPos(const Vec2& pos) const;
public:
	void onEnter(const CameraContext& ctx);
	CameraEvents update(const CameraContext& ctx);
	Vec2 center() const;
};