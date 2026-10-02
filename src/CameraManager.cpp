#include "Camera.h"
#include "CameraTypes.h"
#include "CameraManager.h"

#include <SFML/Graphics/View.hpp>

#include <map>
#include <memory>
#include <cassert>
#include <utility>

void CameraManager::init(const sf::View& baseView)
{
	m_view = baseView;
}

void CameraManager::add(CameraId id, std::unique_ptr<Camera> camera)
{
	assert(camera && "null camera");
	assert((m_cameras.find(id) == m_cameras.end()) && "camera id registered twice");
	m_cameras.emplace(id, std::move(camera));
}

void CameraManager::setActive(CameraId id, const CameraContext& ctx)
{
	auto& camera = *m_cameras.at(id);
	camera.onEnter(ctx);
	m_active = &camera;
	syncView();
}

const sf::View& CameraManager::view() const
{
	return m_view;
}

void CameraManager::setViewport(const sf::FloatRect& viewport)
{
	m_view.setViewport(viewport);
}

// copy the camera's new center into m_view
void CameraManager::syncView()
{
	if (!m_active) return;
	auto c = m_active->center();
	m_view.setCenter({ c.x, c.y });
}

CameraEvents CameraManager::update(const CameraContext& ctx)
{
	if (!m_active) return {};
	auto ev = m_active->update(ctx);
	syncView();
	return ev;
}