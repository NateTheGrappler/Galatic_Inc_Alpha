#include "Scene.h"


Scene::Scene(Game& gameEngine)
	:m_game(gameEngine)
{
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}
void Scene::registerMouseAction(int mouseKey, const std::string& actionName)
{
	m_mouseActionMap[mouseKey] = actionName;
}

void Scene::drawLine(const Vec2f& p1, const Vec2f p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game.window().draw(line, 2, sf::Lines);
}


void Scene::setPaused(bool pausedOrNot)
{
	m_paused = pausedOrNot;
}

size_t Scene::height() const
{
	return m_game.window().getSize().y;
}

size_t Scene::width() const
{
	return m_game.window().getSize().x;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

const ActionMap& Scene::getMouseActionMap() const
{
	return m_mouseActionMap;
}

void Scene::simulate(const size_t frames)
{
	for (size_t i = 0; i < frames; ++i)
	{
		if (!m_paused && !m_hasEnded)
		{
			m_currentFrame++;
			update();
		}
	}
}
