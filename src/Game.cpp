#include "Game.h"
#include "Vec2.h"
#include "Components.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

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
	sFixedCamera();
	sRender();
}

void Game::quit()
{
	m_running = false;
}

void Game::init()
{
	m_window.create(sf::VideoMode({ 768, 512 }), "Magic!", sf::Style::Titlebar | sf::Style::Close);
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
	initFixedCamera();
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

void Game::sPlayerInput()
{
	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			quit();
		}

		// this handler is dead code, window is created with sf::Style::Close
		if (const auto* windowResized = event->getIf<sf::Event::Resized>())
		{
			// this squishes the bg image into the starting window size when resized down
			//sf::FloatRect visibleArea({ 0.0f, 0.0f }, { m_worldSize.x, m_worldSize.y });
			sf::FloatRect visibleArea({ 0.0f, 0.0f }, { static_cast<float>(windowResized->size.x), static_cast<float>(windowResized->size.y) });

			m_window.setView(sf::View(visibleArea));
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

void Game::initFixedCamera()
{
	auto viewSize = m_window.getView().getSize();
	m_cellSize = { viewSize.x, viewSize.y };

	m_gridSize = {
		static_cast<int>(m_worldSize.x / m_cellSize.x),
		static_cast<int>(m_worldSize.y / m_cellSize.y)
	};

	// the world must tile exactly with cells, or the view could show void
	assert(std::fmod(m_worldSize.x, m_cellSize.x) == 0.f);
	assert(std::fmod(m_worldSize.y, m_cellSize.y) == 0.f);

	m_currentCell = cellFromPos(m_player->getComponent<CTransform>().value().pos);
	applyCameraCell();
}

sf::Vector2i Game::cellFromPos(const Vec2& pos) const
{
	int col = static_cast<int>(std::floor(pos.x / m_cellSize.x));
	int row = static_cast<int>(std::floor(pos.y / m_cellSize.y));

	// clamp: a position outside the world still maps to a valid cell
	return {
		std::clamp(col, 0, m_gridSize.x - 1),
		std::clamp(row, 0, m_gridSize.y - 1)
	};
}

Vec2 Game::cellCenter(sf::Vector2i cell) const
{
	// cells are positioned by top-left, the view by center, hence the + 0.5
	return { (cell.x + 0.5f) * m_cellSize.x, (cell.y + 0.5f) * m_cellSize.y };
}

void Game::applyCameraCell()
{
	auto view = m_window.getView();
	auto center = cellCenter(m_currentCell);
	view.setCenter({ center.x, center.y });
	m_window.setView(view);
}

void Game::sFixedCamera()
{
	auto& pTransform = m_player->getComponent<CTransform>().value();
	const auto& pBox = m_player->getComponent<CBoundingBox>().value();

	// player center leaving the cell == more than half the box beyond edge
	auto newCell = cellFromPos(pTransform.pos);
	if (newCell == m_currentCell) return;

	constexpr float margin = 4.f;

	int dx = newCell.x - m_currentCell.x;
	int dy = newCell.y - m_currentCell.y;

	if (dx != 0)
	{
		float dir = dx > 0 ? 1.f : -1.f;
		// shared edge: new cell's left edge going right, its right edge going left
		float edgeX = (dx > 0 ? newCell.x : newCell.x + 1) * m_cellSize.x;
		pTransform.pos.x = edgeX + dir * (pBox.size.x / 2.f + margin);
	}

	if (dy != 0)
	{
		float dir = dy > 0 ? 1.f : -1.f;
		// shared edge: new cell's left edge going right, its right edge going left
		float edgeY = (dy > 0 ? newCell.y : newCell.y + 1) * m_cellSize.y;
		pTransform.pos.y = edgeY + dir * (pBox.size.y / 2.f + margin);
	}

	m_currentCell = newCell;
	applyCameraCell();
}