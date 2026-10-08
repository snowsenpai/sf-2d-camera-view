#include "CameraTypes.h"
#include "FixedCamera.h"
#include "Vec2.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>

void FixedCamera::onEnter(const CameraContext& ctx)
{
	m_cellSize = { ctx.viewSize.x, ctx.viewSize.y };
	
	m_gridSize = {
		static_cast<int>(ctx.worldSize.x / m_cellSize.x),
		static_cast<int>(ctx.worldSize.x / m_cellSize.y)
	};

	// the world must tile exactly with cells, or the view could show void
	assert(std::fmod(ctx.worldSize.x, m_cellSize.x) == 0.f);
	assert(std::fmod(ctx.worldSize.y, m_cellSize.y) == 0.f);

	m_currentCell = cellFromPos(ctx.targetPos);
}

Vec2 FixedCamera::center() const
{
	// cells are positioned by top-left, the view by center, hence the + 0.5
	return { (m_currentCell.x + 0.5f) * m_cellSize.x, (m_currentCell.y + 0.5f) * m_cellSize.y };
}

CameraEvents FixedCamera::update(const CameraContext& ctx)
{
	auto ev = CameraEvents{};
	auto newCell = cellFromPos(ctx.targetPos);
	if (newCell == m_currentCell) return ev;

	int dx = newCell.x - m_currentCell.x;
	int dy = newCell.y - m_currentCell.y;

	if (dx != 0)
	{
		ev.x = EdgeCrossing{
			(dx > 0 ? newCell.x : newCell.x + 1) * m_cellSize.x,
			dx > 0 ? 1 : -1
		};
	}

	if (dy != 0)
	{
		ev.y = EdgeCrossing{
			(dy > 0 ? newCell.y : newCell.y + 1) * m_cellSize.y,
			dy > 0 ? 1 : -1
		};
	}

	m_currentCell = newCell;
	return ev;
}

sf::Vector2i FixedCamera::cellFromPos(const Vec2& pos) const
{
	int col = static_cast<int>(std::floor(pos.x / m_cellSize.x));
	int row = static_cast<int>(std::floor(pos.y / m_cellSize.y));

	// clamp: a position outside the world still maps to a valid cell
	return {
		std::clamp(col, 0, m_gridSize.x - 1),
		std::clamp(row, 0, m_gridSize.y - 1)
	};
}

void FixedCamera::debugDraw(sf::RenderTarget& target) const
{

	auto cellCenter = center();

	auto r = sf::RectangleShape({ 16.f,16.f });
	r.setOrigin({ 8.f,8.f });
	r.setPosition({ cellCenter.x, cellCenter.x });
	r.setFillColor(sf::Color::Transparent);
	r.setOutlineColor(sf::Color::Cyan);
	r.setOutlineThickness(1.f);
	target.draw(r);
}