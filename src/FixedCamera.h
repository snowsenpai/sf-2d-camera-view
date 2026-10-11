#pragma once

#include "Vec2.h"
#include "CameraTypes.h"
#include "Camera.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

// The base-class<Camera> defaults ignore zoom, which is the “fixed camera ignores zoom”
class FixedCamera : public Camera
{
	Vec2 m_cellSize; // one cell == one view
	sf::Vector2i m_gridSize; // cells per axis (cols, rows), derived from world / cell
	sf::Vector2i m_currentCell; // the cell the camera is currently showing
	enum class State { Idle, Sliding };
	State m_state = State::Idle;

	Vec2  m_center;                   // stored now: during a slide it isn't the current cell's center
	Vec2  m_slideFrom, m_slideTo;
	float m_slideElapsed = 0.f;
	static constexpr float kSlideDuration = 0.45f;   // seconds, tune it

	Vec2 cellCenter(sf::Vector2i cell) const;
	sf::Vector2i cellFromPos(const Vec2& pos) const;

	void debugDraw(sf::RenderTarget& target) const override;
public:
	void onEnter(const CameraContext& ctx) override;
	CameraEvents update(const CameraContext& ctx) override;
	Vec2 center() const override;

	bool blocksGameplay() const override { return m_state == State::Sliding; }
};