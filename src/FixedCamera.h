#pragma once

#include "Vec2.h"
#include "CameraTypes.h"
#include "Camera.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class FixedCamera : public Camera
{
	Vec2 m_cellSize; // one cell == one view
	sf::Vector2i m_gridSize; // cells per axis (cols, rows), derived from world / cell
	sf::Vector2i m_currentCell; // the cell the camera is currently showing

	sf::Vector2i cellFromPos(const Vec2& pos) const;

	void debugDraw(sf::RenderTarget& target) const override;
public:
	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;
};