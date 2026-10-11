#pragma once

#include "Camera.h"
#include "CameraTypes.h"
#include "Vec2.h"

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

	sf::View m_miniView;

	float m_miniRatio		= 3.f; // world ration: show 3x the main view's size
	float m_miniFraction	= .25f; // screen fraction of the game area

	Vec2 m_baseSize;

	void syncView();
public:
	void init(const sf::View& baseView);
	void add(CameraId id, std::unique_ptr<Camera> camera);
	
	void setActive(CameraId id, const CameraContext& ctx);
	void setViewport(const sf::FloatRect& main);

	CameraEvents update(const CameraContext& ctx);
	
	Vec2 baseViewSize() const { return m_baseSize; }
	void zoomBy(float factor);
	bool blocksGameplay() const;

	void debugDraw(sf::RenderTarget& target) const;

	
	const sf::View& view() const;
	const sf::View& miniView() const;
};