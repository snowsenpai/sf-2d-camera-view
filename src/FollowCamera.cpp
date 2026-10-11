#include "CameraTypes.h"
#include "FollowCamera.h"
#include "Vec2.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>

float FollowCamera::catchUp(float rate, float dt)
{
	return 1.f - std::exp(-rate * dt);
}

float FollowCamera::pushAxis(float center, float target, float halfWindow)
{
	const float offset = target - center;

	if (offset > halfWindow)
		return center + (offset - halfWindow);   // target past the far edge: push the camera forward
	if (offset < -halfWindow)
		return center + (offset + halfWindow);   // target past the near edge: push it back

	return center;                               // target inside the window: camera stays put
}

float FollowCamera::clampAxis(float desired, float viewSize, float worldSize)
{
	const float half = viewSize / 2.f;
	const float lowest = half;
	const float highest = worldSize - half;

	if (highest < lowest)
	{
		return worldSize / 2.f; // world is smaller than view: center it
	}
	return std::clamp(desired, lowest, highest);
}

Vec2 FollowCamera::clampToWorld(const Vec2& p, const CameraContext& ctx) const 
{
	// the EFFECTIVE view size: zoom is part of it
	const auto view = Vec2{ ctx.baseViewSize.x / m_zoom, ctx.baseViewSize.y / m_zoom };

	return {
		clampAxis(p.x, view.x, ctx.worldSize.x),
		clampAxis(p.y, view.y, ctx.worldSize.y)
	};
}

void FollowCamera::onEnter(const CameraContext& ctx)
{
	m_center = clampToWorld(ctx.targetPos, ctx); // snap, no push, no smoothing
}

CameraEvents FollowCamera::update(const CameraContext& ctx)
{
	// 1. dead zone, measured from the PREVIOUS center (m_center), not from the target
	const auto pushed = Vec2{
		pushAxis(m_center.x, ctx.targetPos.x, m_config.windowHalfSize.x),
		pushAxis(m_center.y, ctx.targetPos.y, m_config.windowHalfSize.y)
	};

	// 2. world clamp on where we want to be
	const auto desired = clampToWorld(pushed, ctx);

	// 3. smoothing; a rate of 0 would freeze the camera, so treat it as "off"
	const float k = (m_config.smoothing > 0.f) ? catchUp(m_config.smoothing, ctx.deltaTime) : 1.f;
	m_center.x += (desired.x - m_center.x) * k;
	m_center.y += (desired.y - m_center.y) * k;

	// 4. zoom-out can make the old center invalid for the new, larger view
	m_center = clampToWorld(m_center, ctx);

	return {};
}

Vec2 FollowCamera::center() const { return m_center; }

float FollowCamera::zoom() const { return m_zoom; }

void FollowCamera::zoomBy(float factor)
{
	m_zoom = std::clamp(m_zoom * factor, kMinZoom, kMaxZoom);
}

void FollowCamera::debugDraw(sf::RenderTarget& target) const
{
	const auto& h = m_config.windowHalfSize;

	auto window = sf::RectangleShape({ h.x * 2.f, h.y * 2.f });
	window.setOrigin({ h.x , h.y });
	window.setPosition({ m_center.x, m_center.y });
	window.setFillColor(sf::Color::Transparent);
	window.setOutlineColor(sf::Color::Cyan);
	window.setOutlineThickness(1.f);
	target.draw(window);
}