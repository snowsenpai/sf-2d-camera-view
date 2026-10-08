#include "CameraTypes.h"
#include "FollowCamera.h"
#include "Vec2.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

float FollowCamera::clampAxis(float desired, float viewSize, float worldSize)
{
	const float half = viewSize / 2.f;
	const float lowest = half;
	const float highest = worldSize - half;

	if (highest < lowest)
	{
		// TODO: how to determine where the view’s left and right edges land at each extreme,
		// and what the player sees as they walk
		// because zoom will make this branch reachable in normal play.
		return std::clamp(desired, highest, lowest);
	}
	return std::clamp(desired, lowest, highest);
}

Vec2 FollowCamera::computeCenter(const CameraContext& ctx) const
{
	return {
		clampAxis(ctx.targetPos.x, ctx.viewSize.x, ctx.worldSize.x),
		clampAxis(ctx.targetPos.y, ctx.viewSize.y, ctx.worldSize.y)
	};
}

void FollowCamera::onEnter(const CameraContext& ctx)
{
	m_center = computeCenter(ctx);
}

CameraEvents FollowCamera::update(const CameraContext& ctx)
{
	m_center = computeCenter(ctx);

	return {};
}

Vec2 FollowCamera::center() const
{
	return m_center;
}

void FollowCamera::debugDraw(sf::RenderTarget& target) const
{
	auto r = sf::RectangleShape({ 16.f,16.f });
	r.setOrigin({ 8.f,8.f });
	r.setPosition({ m_center.x,m_center.x });
	r.setFillColor(sf::Color::Transparent);
	r.setOutlineColor(sf::Color::Cyan);
	r.setOutlineThickness(1.f);
	target.draw(r);
}