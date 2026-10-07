#include "Vec2.h"
#include "CameraTypes.h"
#include "FollowCamera.h"

#include <algorithm>

float FollowCamera::clampAxis(float desired, float viewSize, float worldSize)
{
	const float half = viewSize / 2.f;
	const float lowest = half;
	const float highest = worldSize - half;

	if (highest < lowest)
	{
		return std::clamp(desired, highest, lowest);
	}
	return std::clamp(desired, lowest, highest);
}

Vec2 FollowCamera::computeCenter(const CameraContext& ctx)
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