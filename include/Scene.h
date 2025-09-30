#pragma once

#include "Action.hpp"
#include "EntityManager.hpp"
#include "GameEngine.h"

#include <memory>



typedef std::map<int, std::string> ActionMap;

class Scene
{
protected:

	Game&          m_game;
	EntityManager  m_entityManager;
	ActionMap      m_actionMap;
	ActionMap      m_mouseActionMap;
	bool           m_paused = false;
	bool           m_hasEnded = false;
	size_t         m_currentFrame = 0;

	virtual void onEnd() = 0;
	void setPaused(bool paused);

public:

	Scene() = default;
	Scene(Game& gameEngine);

	//deconstructor for some reason
	virtual ~Scene() = default;

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	void simulate(const size_t frames);
	void registerAction(int inputKey, const std::string& actionName);
	void registerMouseAction(int mouseInput, const std::string& actionName);

	size_t width() const;
	size_t height() const;
	size_t currentFrame() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	const ActionMap& getMouseActionMap() const;
	void drawLine(const Vec2f& p1, const Vec2f p2);

};