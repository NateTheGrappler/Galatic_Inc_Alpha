#pragma once
#include "Scene.h"
#include <map>
#include <memory>
#include "EntityManager.hpp"

class Scene_Create : public Scene
{
	sf::Text                   m_gridText;
	std::shared_ptr<Entity>    m_currentEntity;
	std::shared_ptr<Entity>    m_previousEntity;
	std::string                m_newlevelName = "";
	char                       m_textBuffer[256] = "";
	const Vec2f                m_gridSize = { 64, 64 };
	bool                       m_drawGrid = true;
	bool                       m_mouseDrag = false;
	bool                       m_snapToGrid = true;
	bool                       m_dragCreate = false;
	bool                       m_dragToDelete = false;
	bool                       m_drawCollision = false;
	bool                       m_showFPS = false;
	sf::Vector2f               m_oldPos;
	std::string                m_fileToBeDeleted = "";
	float                      m_windowDifferenceY = 0;
	


protected:
	void init();
	void saveLevelToFile(std::string fileName);
	void loadLevel(const std::string& filename);
	std::string getLevelsDirectory();
	const std::string checkTypeOfTile(const std::string& tag);

	void update();
	void onWindowResize();
	void onEnd() override;
	void sDoAction(const Action& action) override;
	void sRender() override;
	void sGUI();

	void drawGrid();
	void initObjectToMouse(const std::string name);
	void dragCreateInit();
	void UpdatePosition(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> currentPickUp();

	Vec2f midPixelToGrid(float mouseX, float mouseY) const;

	Vec2f worldToGridCoordinates(const Vec2f& worldPos) const;
	Vec2f gridToWorldCoordinates(const Vec2f& gridPos, std::shared_ptr<Entity> entity) const;

	Vec2f gridToMidPixel(float x, float, std::shared_ptr<Entity> entity);
	void mouseCollision(std::string actionName);
	bool checkIfEmptyCell(std::shared_ptr<Entity> entity);


public:

	//A instance declaration that is used to a given gameEngine refrence
	Scene_Create(Game& gameEngine);
};
