#include "Camera.h"
#include "CameraManager.h"
#include "CameraTypes.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>

#include <cassert>
#include <map>
#include <memory>
#include <utility>

void CameraManager::init(const sf::View& baseView)
{
	m_view = baseView;
	m_baseSize = { baseView.getSize().x, baseView.getSize().y };
}

bool CameraManager::blocksGameplay() const
{
	return m_active && m_active->blocksGameplay();
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

	if (m_active == &camera) return;
	
	camera.onEnter(ctx);
	m_active = &camera;
}

const sf::View& CameraManager::view() const
{
	return m_view;
}

const sf::View& CameraManager::miniView() const
{
	return m_miniView;
}

void CameraManager::setViewport(const sf::FloatRect& main)
{
	m_view.setViewport(main);

	constexpr float margin = 0.02f; // of the game area, so it can't land in the letterbox bars
	sf::FloatRect mini;
	mini.size = {
		main.size.x * m_miniFraction,
		main.size.y * m_miniFraction,
	};
	mini.position = {
		main.position.x + main.size.x * (1.f - m_miniFraction - margin), // right side of the game area
		main.position.y + main.size.y * margin // top
	};
	m_miniView.setViewport(mini);
}

void CameraManager::zoomBy(float factor)
{
	if (m_active) m_active->zoomBy(factor);
}

// CameraManager::syncView: absolute size, not View::zoom(), which is relative and drifts
void CameraManager::syncView()
{
	if (!m_active) return;
	
	const auto c = m_active->center();
	const auto z = m_active->zoom();
	const auto size = sf::Vector2f{ m_baseSize.x / z,m_baseSize.y / z };
	const auto half = size / 2.f;

	m_view.setSize(size);
	// Pixel alignment: snap the view's TOP-LEFT to whole pixels, not the center.
	// With a fractional size (zoom), only the top-left stays aligned.
	m_view.setCenter({ std::round(c.x - half.x) + half.x, std::round(c.y - half.y) + half.y });

	m_miniView.setCenter(m_view.getCenter());
	m_miniView.setSize({ size.x * m_miniRatio, size.y * m_miniRatio });
}

CameraEvents CameraManager::update(const CameraContext& ctx)
{
	if (!m_active) return {};
	auto ev = m_active->update(ctx);
	syncView();
	return ev;
}


void CameraManager::debugDraw(sf::RenderTarget& target) const
{
	m_active->debugDraw(target);
}