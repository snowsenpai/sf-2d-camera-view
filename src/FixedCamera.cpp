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
	m_cellSize = { ctx.baseViewSize.x, ctx.baseViewSize.y };
	
	m_gridSize = {
		static_cast<int>(ctx.worldSize.x / m_cellSize.x),
		static_cast<int>(ctx.worldSize.y / m_cellSize.y)
	};

	// the world must tile exactly with cells, or the view could show void
	assert(std::fmod(ctx.worldSize.x, m_cellSize.x) == 0.f);
	assert(std::fmod(ctx.worldSize.y, m_cellSize.y) == 0.f);

	m_currentCell = cellFromPos(ctx.targetPos);
	m_center = cellCenter(m_currentCell);
	m_state = State::Idle; // cancels any slide left over from last time
}

Vec2 FixedCamera::cellCenter(sf::Vector2i cell) const
{
	return { (cell.x + 0.5f) * m_cellSize.x, (cell.y + 0.5f) * m_cellSize.y };
}

Vec2 FixedCamera::center() const { return m_center; }

CameraEvents FixedCamera::update(const CameraContext& ctx)
{
	CameraEvents ev;

	if (m_state == State::Sliding)
	{
		m_slideElapsed += ctx.deltaTime;
		const float t = std::min(m_slideElapsed / kSlideDuration, 1.f);
		const float e = t * t * (3.f - 2.f * t);               // smoothstep easing

		m_center = {
			m_slideFrom.x + (m_slideTo.x - m_slideFrom.x) * e,
			m_slideFrom.y + (m_slideTo.y - m_slideFrom.y) * e
		};

		if (t >= 1.f) m_state = State::Idle;
		return ev;                                             // crossings are ignored while sliding
	}

	const sf::Vector2i newCell = cellFromPos(ctx.targetPos);
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

	m_slideFrom = m_center;
	m_slideTo = cellCenter(newCell);
	m_slideElapsed = 0.f;
	m_state = State::Sliding;
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
	auto cell = sf::RectangleShape({ m_cellSize.x, m_cellSize.y });
	cell.setPosition({ m_currentCell.x * m_cellSize.x, m_currentCell.y * m_cellSize.y });
	cell.setFillColor(sf::Color::Transparent);
	cell.setOutlineColor(sf::Color::Cyan);
	cell.setOutlineThickness(-2.f);                            // negative: drawn inward, stays inside the cell
	target.draw(cell);
}