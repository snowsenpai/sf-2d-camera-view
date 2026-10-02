#include "Game.h"
#include "Vec2.h"
#include "CameraTypes.h"
#include "CameraManager.h"
#include "FixedCamera.h"
#include "Components.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <iostream>

void Game::run()
{
	init();

	while (m_running)
	{
		update();
	}
	m_window.close();
}

void Game::update()
{
	m_entityManager.update();

	sPlayerInput();
	sMovement();
	sCollision();
	sCamera();
	sRender();
}

void Game::quit()
{
	m_running = false;
}

void Game::init()
{
	m_window.create(sf::VideoMode({ 768, 512 }), "Magic!", sf::Style::Default);
	m_window.setFramerateLimit(60);

	constexpr auto bgPath = "assets/fantasy_world_map1.png";
	
	if (!m_bgTexture.loadFromFile(bgPath))
	{
		std::cout << "[!] Failed to load background texture" << "\n";
	}

	// this will probably fail if loading texture fails, sfml will throw an error for invalid file paths so...
	m_bgImage.emplace(m_bgTexture);

	auto bgTextureSize = m_bgTexture.getSize();
	m_worldSize = { static_cast<float>(bgTextureSize.x), static_cast<float>(bgTextureSize.y) };
	
	spawnPlayer();

	// CameraManager must be initialized before setting an active camera
	m_cameraManager.init(m_window.getView());
	m_cameraManager.add(CameraId::Fixed, std::make_unique<FixedCamera>());
	m_cameraManager.setActive(CameraId::Fixed, makeCameraContext());

	onResize(m_window.getSize());
	m_running = true;
}

void Game::spawnPlayer()
{
	auto player = m_entityManager.addEntity("player");
	
	float playerRadius = 20.f;
	int shapePoints = 10;
	sf::Color fillColor(sf::Color::Red);
	sf::Color outlineColor(sf::Color::Yellow);
	float thickness = 1.f;

	player->addComponent<CShape>(playerRadius, shapePoints, fillColor, outlineColor, thickness);
	
	// spawn player at a random position within the window
	float maxPosX = static_cast<float>(m_window.getSize().x - playerRadius);
	float maxPosY = static_cast<float>(m_window.getSize().y - playerRadius);
	
	float posX = rng(playerRadius, maxPosX);
	float posY = rng(playerRadius, maxPosY);

	player->addComponent<CTransform>(Vec2(posX, posY));

	float boxSize = playerRadius * 2;
	player->addComponent<CBoundingBox>(Vec2(boxSize, boxSize));
	
	player->addComponent<CInput>();
	
	m_player = player;
}

void Game::sRender()
{
	m_window.clear();

	// draw background
	m_window.draw(m_bgImage.value());

	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CTransform>() || !e->hasComponent<CShape>()) continue;

		auto& entityCircle = e->getComponent<CShape>().value().circle;
		auto& entityPos = e->getComponent<CTransform>().value().pos;
		
		entityCircle.setPosition({ entityPos.x, entityPos.y });

		m_window.draw(entityCircle);
		
		if (m_drawCollisison)
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& eBox = e->getComponent<CBoundingBox>().value().size;
			
				sf::RectangleShape box({ eBox.x, eBox.y });

				box.setOrigin({ eBox.x / 2.f, eBox.y / 2.f });
				box.setPosition({ entityPos.x, entityPos.y });
				box.setOutlineColor(sf::Color::Green);
				box.setFillColor(sf::Color::Transparent);
				box.setOutlineThickness(1.f);
			
				m_window.draw(box);
			}
			else if (e->hasComponent<CBoundingCircle>())
			{
				auto& eCircle = e->getComponent<CBoundingCircle>().value();

				sf::CircleShape circle(eCircle.radius);

				circle.setOrigin({ eCircle.radius, eCircle.radius });
				circle.setPosition({ entityPos.x, entityPos.y });
				circle.setOutlineColor(sf::Color::Green);
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineThickness(1.f);

				m_window.draw(circle);
			}
		}
	}

	m_window.display();
}

void Game::sCollision()
{
	// player circle x window collision (in this case, player's bounding box or circle is not relevant for window collision)
	float playerRadius = m_player->getComponent<CShape>().value().circle.getRadius();
	
	auto& playerTransform = m_player->getComponent<CTransform>().value();
	
	// with 2d scrolling collision is with world bounds not window
	
	// x axis
	if (
		// left
		playerTransform.pos.x - playerRadius <= 0.0f ||
		// right
		playerTransform.pos.x + playerRadius >= m_worldSize.x
		)
	{
		playerTransform.pos.x = playerTransform.prevPos.x;
	}

	// y axis
	if (
		// bottom
		playerTransform.pos.y + playerRadius >= m_worldSize.y ||
		// top
		playerTransform.pos.y - playerRadius <= 0.0f
		)
	{
		playerTransform.pos.y = playerTransform.prevPos.y;
	}
}

void Game::sMovement()
{
	auto& pTransform = m_player->getComponent<CTransform>().value();
	auto& pInput = m_player->getComponent<CInput>().value();

	// reset player velocity on each frame
	pTransform.velocity = { 0.0f, 0.0f };

	float speed = 8.f;
	if (pInput.up)
	{
		pTransform.velocity.y = -speed;
	}
	else if (pInput.down)
	{
		pTransform.velocity.y = speed;
	}
	else if (pInput.left)
	{
		pTransform.velocity.x = -speed;
	}
	else if (pInput.right)
	{
		pTransform.velocity.x = speed;
	}

	// update players position
	pTransform.prevPos = pTransform.pos;
	pTransform.pos += pTransform.velocity;
}

void Game::onResize(sf::Vector2u newSize)
{
	auto viewSize = m_cameraManager.view().getSize();
	const float targetAspect = viewSize.x / viewSize.y;
	const float windowAspect = static_cast<float>(newSize.x) / static_cast<float>(newSize.y);

	auto viewPort = sf::FloatRect{ {0.f, 0.f}, {1.f, 1.f} };

	if (windowAspect > targetAspect)
	{
		// window too wide: bars left and right
		viewPort.size.x = targetAspect / windowAspect;
		viewPort.position.x = (1.f - viewPort.size.x) / 2.f;
	}
	else
	{
		// window too tall: bars top and bottom
		viewPort.size.y = windowAspect / targetAspect;
		viewPort.position.y = (1.f - viewPort.size.y) / 2.f;
	}

	m_cameraManager.setViewport(viewPort);
}

void Game::sPlayerInput()
{
	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			quit();
		}

		if (const auto* windowResized = event->getIf<sf::Event::Resized>())
		{
			onResize(windowResized->size);
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->code)
			{
			case sf::Keyboard::Key::W:
				m_player->getComponent<CInput>().value().up = true;
				break;
			case sf::Keyboard::Key::S:
				m_player->getComponent<CInput>().value().down = true;
				break;
			case sf::Keyboard::Key::A:
				m_player->getComponent<CInput>().value().left = true;
				break;
			case sf::Keyboard::Key::D:
				m_player->getComponent<CInput>().value().right = true;
				break;
			case sf::Keyboard::Key::C:
				m_drawCollisison = !m_drawCollisison;
				break;
			default:
				break;
			}
		}
		else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->code)
			{
			case sf::Keyboard::Key::W:
				m_player->getComponent<CInput>().value().up = false;
				break;
			case sf::Keyboard::Key::S:
				m_player->getComponent<CInput>().value().down = false;
				break;
			case sf::Keyboard::Key::A:
				m_player->getComponent<CInput>().value().left = false;
				break;
			case sf::Keyboard::Key::D:
				m_player->getComponent<CInput>().value().right = false;
				break;
			default:
				break;
			}
		}
	}
}

void Game::sCamera()
{
	auto ev = m_cameraManager.update(makeCameraContext());
	onCameraEvent(ev);
	m_window.setView(m_cameraManager.view());
}

CameraContext Game::makeCameraContext() const
{
	auto view = m_window.getView().getSize();
	const auto& pos = m_player->getComponent<CTransform>().value().pos;
	return { pos, m_worldSize, {view.x, view.y} };
}

void Game::onCameraEvent(const CameraEvents& ev)
{
	auto& t = m_player->getComponent<CTransform>().value();
	const auto& box = m_player->getComponent<CBoundingBox>().value();

	if (ev.x)
	{
		t.pos.x = ev.x->edge + ev.x->dir * (box.size.x / 2.f + kRoomMargin);
	}
	if (ev.y)
	{
		t.pos.y = ev.y->edge + ev.y->dir * (box.size.y / 2.f + kRoomMargin);
	}
}