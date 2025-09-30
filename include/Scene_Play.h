//Yeah good luck buddy
#pragma once

#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.hpp"

class Scene_Play : public Scene
{
	//A player config which you will have to read in from another file (file has yet to be made)
	struct PlayerConfig
	{
		float X, Y, CX, CY, SCALEX, SCALEY, SPEED, MAXSPEED, JUMP, GRAVITY, HEARTS;
	};
	struct EnemyConfig
	{
		float GX, GY, CX, CY, SCALEX, SCALEY, VERTSPEED, MAXSPEED, Health, TurnAroundTime;
		std::string enemyName, enemyAnimationName;
	};
	struct SceneControl
	{
		bool doRender = true;
		bool doMovement = true;
		bool doCollision = true;
		bool doAnimation = true;
		bool doGUI = true;
		bool doLifeSpan = true;
	};

protected:

	std::string             m_levelPath;								//a path to a text file that is supposed to read in where all the entities are supposed to be placed
	PlayerConfig            m_playerConfig;							//A player config that is supposed to be used to read all the player attributes
	EnemyConfig             m_enemyConfig;
	SceneControl            m_sceneControl;
	std::shared_ptr<Entity> m_currentPush;
	std::shared_ptr<Entity> m_currentHold;
	std::shared_ptr<Entity> m_currentThrow;
	bool                    m_drawTextures = true;					//A bool used for rendering in the scene
	bool                    m_drawCollision = false;					//Another bool used for rendering the collisions/bounding boxes in a scene
	bool                    m_drawGrid = false;						//Drawing the 64x64 pixel grid for debugging
	bool                    m_drawRaycast = true;
	const Vec2f             m_gridSize = { 64, 64 };					//size of the grid
	float                   m_deltaTime;
	float                   m_windowDifferenceY = 0;
	bool                    m_showFPS = false;
	sf::Text                m_gridText;								//The coordinates for the grid
	


	//Initilize the scene by reading in from the text file of where everythng should be based on the grid
	void init(const std::string& levelPath);

	//loads the level and spawns in the player and places all entities in place
	void loadLevel(const std::string& filename);

	void update();													//this seems to be the place where you do all the systems such as rendering and what not, making the other one obsolete i thin
	void onEnd();													//Called when the scene is ended, used to change the scenes pretty much
	void spawnPlayer();												//A special function to create the player object based on config
	void spawnBullet(std::shared_ptr<Entity> entity, std::string& idetifier);				//take location of the player and then spawn a bullet (pretty simple) 
	void spawnEnemy();
	void spawnAttack(std::shared_ptr<Entity> entity);
	void drawGrid();

	//Just personal functions for ease of use
	const std::string checkTypeOfTile(const std::string& tag);
	bool StillPushing(std::shared_ptr<Entity> player, std::shared_ptr<Entity> block) const;
	void putDownBlock();
	void spawnDustCloud(std::shared_ptr<Entity> entity);

	//basically takes a grid coordinate, and returns an sfml grid pixel coordinate, used for centering the assets
	Vec2f gridToMidPixel(float x, float, std::shared_ptr<Entity> entity);

	//return the shared pointer for player so it can be edited/viewed
	std::shared_ptr<Entity> player();

	void sDoAction(const Action& action) override;					//An override from the scene class, as it basically takes in input
	void sRender() override;										//An override from the scene class, it draws everything

	void sMovement();												//handles all entity movement
	void sLifespan();												//handles all entity destruction (sorta)
	void sCollision();												//handles the phyiscs and collision
	void sAnimation();												//handles drawing and keeping track of the animations
	void sGUI();													//An ImGui gui for debugging
	void sRayCast();

public:

	//A instance declaration that is used to a given gameEngine refrence, and the level path for level creation
	Scene_Play(Game& gameEngine, const std::string levelPath);
};
