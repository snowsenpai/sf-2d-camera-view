#pragma once

#include "Camera.h"
#include "CameraTypes.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>

#include <map>
#include <memory>

enum class CameraId
{
	Fixed,
	Follow
};

class CameraManager
{
	std::map<CameraId, std::unique_ptr<Camera>> m_cameras;
	Camera* m_active = nullptr; // non-owning, points to one of the above
	sf::View m_view;

	void syncView();
public:
	void init(const sf::View& baseView);
	void add(CameraId id, std::unique_ptr<Camera> camera);
	
	void setActive(CameraId id, const CameraContext& ctx);
	void setViewport(const sf::FloatRect& viewport);

	CameraEvents update(const CameraContext& ctx);

	void debugDraw(sf::RenderTarget& target) const;
	
	const sf::View& view() const;
};