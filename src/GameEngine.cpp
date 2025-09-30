//Look through it to see if it needs to be revised (add systems maybe)

#include "GameEngine.h"
#include "Assets.hpp"
#include "Scene_Play.h"
#include "Scene_Menu.h"
#include <iostream>



//The main class constructor, which takes the assets path
Game::Game(const std::string& path)
{
	//Uses that same path to run the init function on the start/creation of the object
	init(path);
}

void Game::init(const std::string& path)
{
	//The assets object class is then used to read in and store all the assets(filepaths to textures) in a single class
	m_assets.loadFromFile(path);

	//Base stuff for the rendering of an sfml window
	m_window.create(sf::VideoMode(1280, 768), "Galactic Ink"); // finish this
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);
	ImGui::SFML::Init(m_window);

	////scale the imgui ui and text size by 2
	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;

	m_framerateText.setFont(m_assets.getFont("retroFont"));
	m_framerateText.setCharacterSize(12);
	m_framerateText.setFillColor(sf::Color::White);
	m_framerateText.setPosition({ 0, 0 });
	m_framerateText.setString("Current Frame Rate: " + std::to_string(m_currentFPS));

	//Basically adds a new scene to the scene class of Scene Menu (uses some fancy pointer syntax for c++)
	changeScene("MENU", std::make_shared<Scene_Menu>(*this), false);
}

//returns the current scene object
std::shared_ptr<Scene> Game::currentScene()
{
	return m_sceneMap[m_currentScene];
}

//a check to see if the game is still running or not
bool Game::isRunning()
{
	return m_running and m_window.isOpen();
}

//returns the window object as a refrence, so that way it can be edited when called in another class
sf::RenderWindow& Game::window()
{
	return m_window;
}

//basically the entry point, a while look to sort of call another while loop
void Game::run()
{
	while (isRunning())
	{
		m_frameCount += 1;
		updateFPS();
		currentScene().get()->update();
	}

	
}

//quit is used to end the program, hence the double while loop, as I can control the outer one with this
void Game::quit()
{
	m_running = false;
	m_window.close();
}

//returns the assets class which holds all the data structures which have the rendering data
Assets& Game::assets()
{
	return m_assets;
}

//TODO:   Figure out what this is supposed to really do
//I think it has to do something with the actual rendering of the window/loading in the actual scene


//Shows a little bit in the 2024 lecture if ya wanna look man
void Game::sUserInput() 
{
	sf::Event event;

	while (m_window.pollEvent(event)) 
	{
		ImGui::SFML::ProcessEvent(event);
		if (ImGui::GetIO().WantCaptureMouse) 
		{
			m_mouseInScene = false;
			continue; 
		}
		else
		{
			m_mouseInScene = true;
		}
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		if (event.type == sf::Event::Resized)
		{
			//Normalize the view by changing size and position so that it remains in proper place without stretching or moving
			sf::View newView(sf::FloatRect(0, 0, event.size.width, event.size.height));
			m_window.setView(newView);
			
			//If the current scene does not have an action for the key, then skip this iteration
			if (currentScene()->getActionMap().find(sf::Event::Resized) == currentScene()->getActionMap().end())
			{
				continue;
			}
			//determine if the action is a start or end based on whether the key is pressed or released
			const std::string actionType = "START";

			//look up the action and send the action to the scene
			Action action(currentScene()->getActionMap().at(sf::Event::Resized), actionType);
			currentScene()->sDoAction(action);
		}
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased ) 
		{
			//If the current scene does not have an action for the key, then skip this iteration
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue;
			}
			//determine if the action is a start or end based on whether the key is pressed or released
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
		
			//look up the action and send the action to the scene
			Action action(currentScene()->getActionMap().at(event.key.code), actionType);
			currentScene()->sDoAction(action);
		
		}
		else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
		{
			if (currentScene()->getMouseActionMap().find(event.key.code) == currentScene()->getMouseActionMap().end())
			{
				continue;
			}
			const std::string actionType = (event.type == sf::Event::MouseButtonPressed) ? "START" : "END";

			//look up the action and send the action to the scene
			Action action(currentScene()->getMouseActionMap().at(event.mouseButton.button), actionType);
			currentScene()->sDoAction(action);
		}
		else if (event.type == sf::Event::MouseWheelMoved)
		{
			if (currentScene()->getMouseActionMap().find(event.MouseWheelScrolled) == currentScene()->getMouseActionMap().end())
			{
				continue;
			}
			std::string actionType;
			if (event.mouseWheel.delta > 0)
			{
				actionType = "ZoomIn";
			}
			else if (event.mouseWheel.delta < 0)
			{
				actionType = "ZoomOut";
			}
			
			Action action(currentScene()->getMouseActionMap().at(event.MouseWheelScrolled), actionType);
			currentScene()->sDoAction(action);
		}
	}
}

//This basically takes in a scene name, as well as a pointer to the scene, as well as if you should end the current scene
//Earlier when using it for Scene_Menu, you didnt need to close the current scene, so it just added the scene into it
void Game::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{

	if (endCurrentScene)
	{
		m_sceneMap.erase(m_currentScene);
	}
	m_sceneMap[sceneName] = scene;
	m_currentScene = sceneName;
}

sf::Clock& Game::deltaClock()
{
	return m_deltaClock;
}

bool Game::mouseInWindow() 
{
	return m_mouseInScene;
}


void Game::updateFPS()
{
	// Update FPS every second
	if (m_deltaClock.getElapsedTime().asSeconds() >= 1.0f)
	{
		m_currentFPS = m_frameCount / m_deltaClock.restart().asSeconds();
		m_frameCount = 0;
	}
	m_framerateText.setString("Current Frame Rate: " + std::to_string(m_currentFPS));
	m_frameCount++;
}

sf::Text& Game::getFPS()
{
	return m_framerateText;
}