#include "Vec2.h"
#include "CameraTypes.h"
#include "FollowCamera.h"

void FollowCamera::onEnter(const CameraContext& ctx)
{
	m_center = ctx.targetPos;
}

CameraEvents FollowCamera::update(const CameraContext& ctx)
{
	m_center = ctx.targetPos;
	return {};
}

Vec2 FollowCamera::center() const
{
	return m_center;
}