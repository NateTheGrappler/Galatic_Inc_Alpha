//Definietly some heavy editing might be here, not fully done

#pragma once
#include "Assets.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>

enum class ArialStates { base, jump, grounded,  };
enum class States { base, dead, pushing, run, hurt, attack, shoot,
					doubleJump, stand, grounded, NONgrounded, jumpSpin, 
					notDying, dying, abduct, idle, longDead, flying, 
					hit, died, walk, death, holding, holdingJump, holdingBox,
					falling};

class Scene;

using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

class Game
{
protected:
	sf::RenderWindow  m_window;					//The window that is drawn to
	Assets            m_assets;					//Holds and reads all the assets 
	std::string       m_currentScene;			//The name of a scene
	SceneMap          m_sceneMap;				//A string and then a shared pointer to a scene class, so you can access the scene via the game class
	size_t            m_simulationSpeed = 1;
	sf::Clock         m_deltaClock;				//Clock class for time passage
	bool              m_mouseInScene;
	bool              m_running = true;			//Basically if were running or not (used for pause or quit)
	int               m_frameCount = 0;
	float             m_currentFPS = 0.0f;
	sf::Text          m_framerateText;


	void init(const std::string& config);		//intialize the GameState with a config assets file
	std::shared_ptr<Scene> currentScene();	    //This returns a shared pointer to the scene object of the current scene

public:
	std::string       createdLevelsPath;

	//A constructor that takes a file path to a config text file	
	Game(const std::string& configPath);

	//Basically a function that updates the SceneMap by erasing the current scene from the map and appending a new one
	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene);
	
	void sUserInput();						    //takes in the user input I believe

	void quit();								//quit the application (close window)
	void run();									//The inital entry point, and mostly calls update
	
	sf::RenderWindow& window();					//return the sfml style window object
	sf::Clock& deltaClock();
	Assets& assets();				//Returns the assets class which holds every asset
	bool isRunning();							//checks to see if running (returns m_running)
	bool mouseInWindow();
	void updateFPS();
	sf::Text& getFPS();

};