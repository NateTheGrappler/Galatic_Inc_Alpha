#pragma once
#include "Scene_Create.h"
#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Components.hpp"
#include "Action.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <unordered_set>
#ifdef _WIN32
#include <windows.h>
#endif
#include <shlobj.h>


Scene_Create::Scene_Create(Game& gameEngine) 
    : Scene(gameEngine) 
{
    init();
}

void Scene_Create::init()
{
	registerAction(sf::Keyboard::Escape,               "ESCAPE");
	registerAction(sf::Keyboard::BackSpace,            "DELETE");
	registerAction(sf::Keyboard::G,                    "TOGGLE_GRID");         //Toggle drawing (G)rid
	registerAction(sf::Keyboard::C,                    "TOGGLE_COLLISION");
	registerAction(sf::Event::Resized,                 "RESIZED");
	registerMouseAction(sf::Mouse::Left,               "PLACE");
	registerMouseAction(sf::Mouse::Right,              "MOVECAMERA");
	registerMouseAction(sf::Mouse::Middle,             "PICKUP");
	registerMouseAction(sf::Event::MouseWheelScrolled, "MouseScrolled");
	//TODO: Register all gameplay Actions for the sDoAction Function Later

	//Set all the parameters you want for the grid
	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game.assets().getFont("GridFont"));

}

void Scene_Create::saveLevelToFile(std::string lvlName)
{
	std::string levelsDir = getLevelsDirectory();
	auto filePath = levelsDir + lvlName + ".txt";

	std::ofstream file(filePath);


	if (file.is_open())
	{
		// Write your level data here
		// Example: file << "Level data content";
		int entityCounter = 0;
		//SIGHHHHHHHHHHHH
		//While this could have been done in a single while loop, the drawing order would have been dependent
		//Based on the time the user placed the entity in refrence to all of the others, this leads to bad visuals
		//And so instead we iterate over the array, searching for what we want, so it can all be drawn in order

		//Background
		for (auto& e : m_entityManager.getEntities())
		{

			auto& animation = e->get<CAnimation>();

			if (animation.animation.getName() != "lvl1BG" 
				&& animation.animation.getName() != "MainMenuBG"
				&& animation.animation.getName() != "MercuryBG"
				&& animation.animation.getName() != "VenusBG")
			{ continue; }
			entityCounter += 1;

			auto& transform = e->get<CTransform>();
			auto gridPos = worldToGridCoordinates(transform.pos);

			file
				<< "DecBG ";

			if (entityCounter == m_entityManager.getEntities().size())
			{
				file << animation.animation.getName();
			}
			else
			{
				file << animation.animation.getName() << "\n";
			}
		}

		//Decorations
		for (auto& e : m_entityManager.getEntities())
		{

			auto& animation = e->get<CAnimation>();

			if (animation.animation.getName() != "KeyA" 
				&& animation.animation.getName() != "KeyS" 
				&& animation.animation.getName() != "KeyD"
				&& animation.animation.getName() != "KeyW"
				&& animation.animation.getName() != "Moon_Haunts_You"
				&& animation.animation.getName() != "LeftClick"
				&& animation.animation.getName() != "Right"
				&& animation.animation.getName() != "ShiftKey"
				&& animation.animation.getName() != "planet"
				&& animation.animation.getName() != "pointSign"
				&& animation.animation.getName() != "ShipIdle"
				&& animation.animation.getName() != "pointSignUp")
			
			{ continue; }
			entityCounter += 1;

			auto& transform = e->get<CTransform>();
			//Vec2f transformPos = normalizePosition(e);
			auto gridPos = worldToGridCoordinates(transform.pos);


			file
				<< "Dec "
				<< animation.animation.getName() << " "
				<< gridPos.x << " "
				<< gridPos.y << " "
				<< transform.scale.x << " "
				<< transform.scale.y << " ";

			if (entityCounter == m_entityManager.getEntities().size())
			{
				file << animation.repeat;
			}
			else
			{
				file << animation.repeat << "\n";
			}
		}

		//Player
		for (auto& e : m_entityManager.getEntities())
		{
			auto& animation = e->get<CAnimation>();

			if (animation.animation.getName() != "Stand") { continue; }
			entityCounter += 1;

			auto& transform = e->get<CTransform>();
			auto gridPos = worldToGridCoordinates(transform.pos);
			auto& bBox = e->get<CBoundingBox>();
			file
				<< "Player "
				<< gridPos.x << " "
				<< gridPos.y << " "
				<< bBox.size.x / transform.scale.x << " "
				<< bBox.size.y / transform.scale.x << " "
				<< transform.scale.x << " "
				<< transform.scale.y << " "
				<< "10" << " "
				<< "18" << " "
				<< "3" << " "
				<< "1.3" << " ";
			if (entityCounter == m_entityManager.getEntities().size())
			{
				file << "5";
			}
			else
			{
				file << "5" << "\n";
			}
		}

		//Enemys
		for (auto& e : m_entityManager.getEntities())
		{

			auto& animation = e->get<CAnimation>();

			if (animation.animation.getName() != "spinIdle" && animation.animation.getName() != "TallIdleSleep") { continue; }
			entityCounter += 1;


			auto& transform = e->get<CTransform>();
			auto gridPos = worldToGridCoordinates(transform.pos);


			auto& bBox = e->get<CBoundingBox>();
			std::string enemyName;

			//SIGHHHHHHHHHHHH
			//Get enemy names for the scenePlay to work properly
			if (animation.animation.getName() == "spinIdle")
			{
				enemyName = "spinShroom";
				gridPos.y += 0.5;
			}
			else if (animation.animation.getName() == "TallIdleSleep")
			{
				enemyName = "TallMushroom";
			}

			file
				<< "Enemy "
				<< gridPos.x << " "
				<< gridPos.y << " "
				<< "64" << " "
				<< "64" << " "
				<< transform.scale.x << " "
				<< transform.scale.y << " "
				<< "8" << " "
				<< "24" << " "
				<< "150" << " "
				<< enemyName << " ";
			if (entityCounter == m_entityManager.getEntities().size())
			{
				file << animation.animation.getName();
			}
			else
			{
				file << animation.animation.getName() << "\n";
			}
		}

		//AllTiles
		for (auto& e : m_entityManager.getEntities())
		{

			auto& animation = e->get<CAnimation>();

			if (animation.animation.getName() != "Dirt" 
				&& animation.animation.getName() != "Rock" 
				&& animation.animation.getName() != "Egg"
				&& animation.animation.getName() != "PSteel"
				&& animation.animation.getName() != "VenusDirt"
				&& animation.animation.getName() != "VenusDirtR"
				&& animation.animation.getName() != "VenusDirtL"
				&& animation.animation.getName() != "VenusDirtML"
				&& animation.animation.getName() != "VenusDirtM"
				&& animation.animation.getName() != "VenusDirtMR"
				&& animation.animation.getName() != "VenusDirtSM"
				&& animation.animation.getName() != "VenusDirtSR"
				&& animation.animation.getName() != "VenusDirtSL"
				&& animation.animation.getName() != "VenusDirtB"
				&& animation.animation.getName() != "VenusDirtLL"
				&& animation.animation.getName() != "VenusDirtLR"
				&& animation.animation.getName() != "VenusMoveAble"
				&& animation.animation.getName() != "MercuryTR"
				&& animation.animation.getName() != "MercuryTM"
				&& animation.animation.getName() != "MercuryTL"
				&& animation.animation.getName() != "MercuryMR"
				&& animation.animation.getName() != "MercuryMM"
				&& animation.animation.getName() != "MercuryML"
				&& animation.animation.getName() != "MercuryBR"
				&& animation.animation.getName() != "MercuryBM"
				&& animation.animation.getName() != "MercuryBL"
				&& animation.animation.getName() != "MercuryInnerTR"
				&& animation.animation.getName() != "MercuryInnerTM"
				&& animation.animation.getName() != "MercuryInnerTL"
				&& animation.animation.getName() != "MercuryInnerML"
				&& animation.animation.getName() != "MercuryInnerMR"
				&& animation.animation.getName() != "MercuryInnerBR"
				&& animation.animation.getName() != "MercuryInnerBM"
				&& animation.animation.getName() != "MercuryInnerBL"
				&& animation.animation.getName() != "MercurySingleF"
				&& animation.animation.getName() != "MercurySingleL"
				&& animation.animation.getName() != "MercurySingleM"
				&& animation.animation.getName() != "MercurySingleR")
			{ continue; }
			entityCounter += 1;

			auto& transform = e->get<CTransform>();
			auto gridPos = worldToGridCoordinates(transform.pos);
			

			file
				<< "Tile "
				<< animation.animation.getName() << " "
				<< gridPos.x << " "
				<< gridPos.y << " "
				<< transform.scale.x << " "
				<< transform.scale.y << " ";

			if (entityCounter == m_entityManager.getEntities().size())
			{
				file << animation.repeat;
			}
			else
			{
				file << animation.repeat << "\n";
			}
		}

		file.close();

	}
	else
	{
		std::cerr << "Error: Could not create file " << filePath << std::endl;
	}
}

void Scene_Create::update()
{
	if (m_fileToBeDeleted != "")
	{
		std::filesystem::remove(m_fileToBeDeleted);
		m_fileToBeDeleted = "";
	}

	m_entityManager.update();                                            //Update entities so handle entity destruction
	ImGui::SFML::Update(m_game.window(), m_game.deltaClock().restart()); //Update SFML GUI Window
	for (auto& e : m_entityManager.getEntities()) {UpdatePosition(e);}   //UpdatePosition Of held Entities

	//These are here to check for an update every time you want to drag create or drag destroy an entity
	if (m_dragCreate && m_snapToGrid && checkIfEmptyCell(currentPickUp())){ dragCreateInit(); }
	if (m_dragToDelete) { mouseCollision("DELETE"); }

	//Run the main systems of the scene
    m_game.sUserInput();
	sGUI();
    sRender();
}

void Scene_Create::onWindowResize()
{
	auto distTileFromOriginalY = m_game.window().getSize().y - m_game.window().getDefaultView().getSize().y;
	float windowCenterX = std::max(m_game.window().getSize().x / 2.0f, m_game.window().getSize().y / 2.0f);
	sf::View mainView = m_game.window().getView();
	mainView.setCenter(windowCenterX, m_game.window().getSize().y / 2.0f - distTileFromOriginalY);
	m_game.window().setView(mainView);
}

void Scene_Create::sRender()
{
	m_game.window().clear(sf::Color(10, 10, 200));


	//Draw all entities based on their position
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->has<CTransform>())
		{
			auto& transform = e->get<CTransform>().pos;
			auto& velocity = e->get<CTransform>().velocity;
			auto& scale = e->get<CTransform>().scale;
			transform.x += velocity.x;
			transform.y += velocity.y;

			if (e->has<CAnimation>())
			{
				auto& sprite = e->get<CAnimation>().animation.getSprite();
				sprite.setScale({ scale.x, scale.y });
				sprite.setPosition({ transform.x, transform.y });
				if (e->isActive()) { m_game.window().draw(sprite); };
			}
		}
	}

	////Draw all entity textures / animations
	if (m_drawCollision)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->has<CBoundingBox>())
			{
				auto& box = e->get<CBoundingBox>();
				auto& transform = e->get<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f({ box.size.x - 1.0f, box.size.y - 1.0f })); //video cuts off
				rect.setOrigin(sf::Vector2f({ box.halfsize.x, box.halfsize.y }));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(252, 30, 5, 100));
				rect.setOutlineColor(sf::Color::Black);
				rect.setOutlineThickness(1);
				m_game.window().draw(rect);
			}
		}
	}

	//If youre dragging the mouse, then change the center of the view to reflect the drag
	if (m_mouseDrag)
	{
		sf::Vector2i mousePos = sf::Mouse::getPosition();
		sf::View mainView = m_game.window().getView();

		// Determine the new position in world coordinates
		const sf::Vector2f newPos = m_game.window().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));
		// Determine how the cursor has moved
		// Swap these to invert the movement direction
		const sf::Vector2f deltaPos = m_oldPos - newPos;

		// Move our view accordingly and update the window
		mainView.setCenter(mainView.getCenter() + deltaPos);
		m_game.window().setView(mainView);

		// Save the new position as the old one
		// We're recalculating this, since we've changed the view
		m_oldPos=m_game.window().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));

	}

	//Draw the white grid lines and numbers
    if (m_drawGrid)
    {
        drawGrid();
    }

	//Draw the accents on the currently picked up object so user knows that it is picked up
	if (m_currentEntity && m_currentEntity->get<CState>().isClicked)
	{
		auto& transform = m_currentEntity->get<CTransform>().pos;
		auto& bbox = m_currentEntity->get<CBoundingBox>();
		
		sf::ConvexShape outLine;
		outLine.setPointCount(4);

		// Draw a red dot at the calculated position
		sf::CircleShape dot(3);
		dot.setFillColor(sf::Color::Red);
		auto pos = m_currentEntity->get<CTransform>().pos;
		dot.setPosition(pos.x - 1.5f, pos.y - 1.5f);
		m_game.window().draw(dot);

		// Update collision shape points
		outLine.setPoint(0, { transform.x - bbox.halfsize.x,  transform.y - bbox.halfsize.y });
		outLine.setPoint(1, { transform.x - bbox.halfsize.x,  transform.y + bbox.halfsize.y });
		outLine.setPoint(2, { transform.x + bbox.halfsize.x,  transform.y + bbox.halfsize.y });
		outLine.setPoint(3, { transform.x + bbox.halfsize.x,  transform.y - bbox.halfsize.y });
		outLine.setOutlineColor(sf::Color::Red);
		outLine.setOutlineThickness(2);
		outLine.setFillColor(sf::Color::Transparent);

		m_game.window().draw(outLine);

	}

	m_windowDifferenceY = m_game.window().getSize().y - m_game.window().getDefaultView().getSize().y;

	//Render SFML Window and IMGUI window:
	ImGui::SFML::Render(m_game.window());
	m_game.window().display();
}

void Scene_Create::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name()      == "TOGGLE_GRID")							   { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "TOGGLE_COLLISION")                               { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "PAUSE")									   { m_paused = !m_paused; }
		else if (action.name() == "ESCAPE")									   { m_game.changeScene("MENU", std::make_shared<Scene_Menu>(m_game), false); onEnd(); }
		else if (action.name() == "QUIT")									   { m_hasEnded = true; }
		else if (action.name() == "PICKUP")                                    { mouseCollision(action.name()); }
		else if (action.name() == "PLACE" && m_currentEntity && !m_dragCreate) { m_dragCreate = true; }
		else if (action.name() == "DELETE"&&!m_currentEntity&&!m_dragToDelete) { m_dragToDelete = true;  }
		else if (action.name() == "RESIZED")								   { onWindowResize(); }
		else if (action.name() == "MOVECAMERA") 
		{
			m_mouseDrag = true; 
			auto mousePos = sf::Mouse::getPosition();
			m_oldPos = m_game.window().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));
		}


	}
	else if (action.type() == "END")
	{
		if (action.name() == "MOVECAMERA") { m_mouseDrag = false; }
		else if (action.name() == "PLACE" && m_currentEntity) 
		{ 
			currentPickUp()->get<CState>().isClicked = false;
			if (m_snapToGrid) { m_currentEntity->destroy(); }
			m_currentEntity = nullptr;
			m_dragCreate = false; 
		}
		else if (action.name() == "DELETE")
		{
			m_dragToDelete = false;
		}
	}

	if (action.type() == "ZoomIn")
	{
		sf::View view = m_game.window().getView();
		view.zoom(0.8f);
		m_game.window().setView(view);

	}
	else if (action.type() == "ZoomOut")
	{
		sf::View view = m_game.window().getView();
		view.zoom(1.2f);
		m_game.window().setView(view);
	}
}

void Scene_Create::sGUI()
{
    ImGui::Begin("My Window");

    if (ImGui::BeginTabBar("CreateTabBar"))
    {
		if (ImGui::BeginTabItem("Guide"))
		{
			ImGui::Text("Hello dear USER! This is the level editor slash");
			ImGui::Text("creator for my game!! Below is how to use it.");
			ImGui::Text("The level creation tool is a bit wonky in terms ");
			ImGui::Text("of how its used, so feel free to refrence");
			ImGui::Text("this guide whenever things goes sideways.");
			
			ImGui::Separator();

			ImGui::Text("Utilities are as follows:  ");
			ImGui::Text("Zoom in and out: Mousewheel");
			ImGui::Text("Toggle grid: G");
			ImGui::Text("Back To main menu: ESC");
			ImGui::Text("Change view: Hold Right Click and move your mouse");

			ImGui::Separator();
			
			ImGui::Text("There are several tabs tabs in this little menu");
			ImGui::Text("and they are as follows: ");
			ImGui::Text("-Sprites: ");
			ImGui::Text("The 'Sprites' tab is the tab with all of" );
			ImGui::Text("the different things in the game. They are not");
			ImGui::Text("organized in any particular order, but when you ");
			ImGui::Text("hover over one it will give you a preview");
			ImGui::Text("of the sprite, as well as some general information ");
			ImGui::Text("like its name and size. In order to draw");
			ImGui::Text("the one of these sprites to the screen, hover ");
			ImGui::Text("over your desired sprite and then left click. ");
			ImGui::Text("doing this will draw the block to the screen at ");
			ImGui::Text("the center of the mouse. The block will follow");
			ImGui::Text("your mouse until you press left click once again. ");
			ImGui::Text("After a block is placed, it can be right clicked in");
			ImGui::Text(" order to be picked up again. While a block is in ");
			ImGui::Text("the clicked state, if you drag left click, the blocks");
			ImGui::Text("will mass fill as you move your mouse. In order");
			ImGui::Text(" to delete a block, hover your mouse over it, and press");
			ImGui::Text("backspace. Backspace can also be held down to mass remove");
			ImGui::Text("blocks.");

			ImGui::Separator();
			ImGui::Text("-Controls: ");
			ImGui::Text("The 'Controls' Tab is the tab that lets you ");
			ImGui::Text("influence many of the entities that exist on your");
			ImGui::Text("screen. This includes the snap to grid mode");
			ImGui::Text(", as well as checking the status of each of your");
			ImGui::Text("entities. You are able to delete, resize, reposition,");
			ImGui::Text(" and pick up entities. This is good for all the little");
			ImGui::Text("details on your level such as scale.");

			ImGui::Separator();
			ImGui::Text("-Save: ");
			ImGui::Text("WARNING: IN ORDER FOR THE LEVEL TO WORK, YOU NEED ");
			ImGui::Text("TO AT LEAST ADD A SINGLE PLAYER ENTITY!!!!");
			ImGui::Text("The 'Save' tab is the tab where youre actually ");
			ImGui::Text("going to be saving and creating your level");
			ImGui::Text("this process is a little bit finnicky, as you ");
			ImGui::Text("first need to name your level. This will be");
			ImGui::Text("this will be the name displayed in the main menu.");
			ImGui::Text("You must first save your level name, and and then");
			ImGui::Text("press the create level button. A brief message will");
			ImGui::Text("appear for a flash. This means that your level has been");
			ImGui::Text(" created, and you can press esc to access it in the ");
			ImGui::Text("main menu!");

			ImGui::Separator();
			ImGui::Text("Thank you so much dear USER, I hope you have fun ");
			ImGui::Text("with my first ever game!! For any further questions ");
			ImGui::Text("or bugs, please contact me at guzinskinatan@gmail.com");


			ImGui::EndTabItem();

		}
        if (ImGui::BeginTabItem("Sprites"))
        {
            ImGui::Text("Available Animations:");

            // Create a grid layout
            static int columns = 4;
            static float uniformSize = 64.0f; // Uniform size for all images
            ImGui::SliderInt("Columns", &columns, 1, 8);
            ImGui::SliderFloat("Image Size", &uniformSize, 32.0f, 128.0f);

            if (ImGui::BeginTable("AnimationGrid", columns, ImGuiTableFlags_SizingStretchSame))
            {
                // Loop over all of the animations to draw/handle the clicks
                int column = 0;
                for (auto& animation : m_game.assets().getAnimations())
                {


					const std::unordered_set<std::string> EXCLUDED_ANIMATIONS = {
					"Died", "Hit", "Hurt", "InkBallFlyH", "InkBallFlyV",
					"InkBallHitH", "InkBallHitV", "Jump", "JumpShoot",
					"JumpSpin", "Run", "Shoot", "TallHurt", "TallDeath",
					"spinAttack", "spinHurt", "tentaIdle", "tentaDied",
					"spinWalk", "spinDeath", "Title", "ShipBeamDown",
					"ShipBeamDownL", "ShipIdleLarge", "BlockFallingP",
					"SquidHoldBox", "JumpParticle", "SquidPushIdle",
					"SquidHoldIdle", "SquidHoldJump", "SquidPushWalk",
					"SquidHoldWalk", "locationTracker"};

					if (EXCLUDED_ANIMATIONS.find(animation.first) != EXCLUDED_ANIMATIONS.end()) {
						continue;
					}


                    // Start new row if needed
                    if (column == 0) {
                        ImGui::TableNextRow();
                    }

                    ImGui::TableSetColumnIndex(column);

                    // Get the texture from the animation's sprite
                    const sf::Texture* texture = animation.second.getSprite().getTexture();
                    if (!texture) 
					{
                        // Skip if no texture is available
                        column = (column + 1) % columns;
                        continue;
                    }

                    // Get the texture size
                    sf::Vector2u textureSize = texture->getSize();

                    // Calculate UV coordinates for the first frame
                    // Assuming your animation stores frame information
                    float frameWidth = 0.0f;
                    float frameHeight = 0.0f;

                    // If you don't have frame size info, you'll need to calculate it
                    // based on your animation system's structure
                    if (frameWidth == 0 || frameHeight == 0) {
                        // Fallback: assume the texture contains multiple frames horizontally
                        // You'll need to adjust this based on your actual animation format
                        int frameCount = animation.second.getFrameCount(); // If available
                        if (frameCount > 0) {
                            frameWidth = textureSize.x / frameCount;
                            frameHeight = textureSize.y;
                        }
                        else {
                            // If no frame info, use the whole texture
                            frameWidth = textureSize.x;
                            frameHeight = textureSize.y;
                        }
                    }

                    // UV coordinates for the first frame (leftmost portion of texture)
                    ImVec2 uv0(0.0f, 0.0f); // Top-left corner of first frame
                    ImVec2 uv1(frameWidth / textureSize.x, 1.0f); // Bottom-right of first frame

                    // Calculate aspect ratio preserving size
                    float aspectRatio = frameWidth / frameHeight;
                    ImVec2 imageSize;
                    if (aspectRatio > 1.0f) {
                        // Wider than tall
                        imageSize = ImVec2(uniformSize, uniformSize / aspectRatio);
                    }
                    else {
                        // Taller than wide or square
                        imageSize = ImVec2(uniformSize * aspectRatio, uniformSize);
                    }

                    // Correct cast for ImTextureID
                    ImTextureID tex_id = static_cast<ImTextureID>(texture->getNativeHandle());

                    // The actual button showing only the first frame
					if (ImGui::ImageButton(animation.first.c_str(), tex_id, imageSize, uv0, uv1))
					{
						for (auto& e : m_entityManager.getEntities())
						{
							e->get<CState>().isClicked = false;
						}

						if (checkTypeOfTile(animation.first) == "MovingPlatform")
						{
							std::cout << "Clicked a MovingPlatform Block" << std::endl;
							ImGui::OpenPopup("Chose Movement Point Count");
						}
						else  // For non-MovingPlatform tiles, initialize immediately
						{
							initObjectToMouse(animation.first);
						}
					}

					// Handle the popup separately - this runs every frame
					ImVec2 centerSprite = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(centerSprite, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal("Chose Movement Point Count", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Please enter and save # of move points for the platform");
						ImGui::Separator();
						if (ImGui::Button("Yes", ImVec2(120, 0)))
						{
							initObjectToMouse(animation.first);
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine();
						ImGui::SetItemDefaultFocus();
						if (ImGui::Button("Cancel", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

                    // Add tooltip on hover
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();

                        // Show a larger preview in the tooltip (first frame only)
                        float tooltipScale = 2.0f;
                        ImVec2 tooltipSize(imageSize.x * tooltipScale, imageSize.y * tooltipScale);
                        ImGui::Image(tex_id, tooltipSize, uv0, uv1);

                        ImGui::Separator();
                        ImGui::Text("%s", animation.first.c_str());
                        ImGui::Text("Texture size: %dx%d", textureSize.x, textureSize.y);
                        ImGui::Text("Frame size: %.0fx%.0f", frameWidth, frameHeight);
                        ImGui::Text("Frames: %d", animation.second.getFrameCount()); // If available
						if (animation.first == "pointSignUp" || animation.first == "pointSign" || animation.first == "spinIdle")
						{
							ImGui::Separator();
							ImGui::Text("WARNING:");
							ImGui::Text("Placing this character with 'SnapToGrid'");
							ImGui::Text("enabled, creates multiple in one spot");
							ImGui::Separator();
							ImGui::Text("Please instead place with freehand!");


						}
						else if (animation.first == "lvl1BG")
						{
							ImGui::Separator();
							ImGui::Text("WARNING:");
							ImGui::Text("Only One background is needed.");
							ImGui::Text("The background follows the Player.");
						}
                        ImGui::EndTooltip();
                    }

                    // Update column counter
                    column = (column + 1) % columns;
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
		if (ImGui::BeginTabItem("Controls"))
		{
			ImGui::Text("Options for all creation Items");
			ImGui::Separator();
			if (ImGui::Checkbox("Snap To Grid", &m_snapToGrid))
			{
			}
			if (ImGui::Button("Clear Current Level"))
			{
				ImGui::OpenPopup("WARNING!!");
			}
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("WARNING!!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Warning, you havent saved this level, opening");
				ImGui::Text("another level will delete your progress here.");
				ImGui::Text("Would you like to open anyways?");
				ImGui::Separator();
				if (ImGui::Button("Yes", ImVec2(120, 0)))
				{
					for (auto& entity : m_entityManager.getEntities())
					{
						entity->destroy();
					}
				}
				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("No", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::Separator();
			ImGui::Text("Active Entities in Scene: %d", m_entityManager.getEntities().size());
			ImGui::Separator();

			if (ImGui::CollapsingHeader("Entity List", ImGuiTreeNodeFlags_DefaultOpen))
			{
				int entityCount = 0;
				for (auto& e : m_entityManager.getEntities())
				{
					if (!e->isActive()) continue;

					entityCount++;
					std::string entityLabel = "Entity " + std::to_string(e->id()) + " - " + e->tag();

					if (ImGui::TreeNode(entityLabel.c_str()))
					{
						// Display entity information
						if (e->has<CTransform>())
						{
							auto& transform = e->get<CTransform>();
							ImGui::Text("Position: (%.1f, %.1f)", transform.pos.x, transform.pos.y);

							// Editable position
							float pos[2] = { transform.pos.x, transform.pos.y };
							if (ImGui::InputFloat2("Edit Position", pos))
							{
								transform.pos.x = pos[0];
								transform.pos.y = pos[1];
							}

							// Editable scale
							float scale[2] = { transform.scale.x, transform.scale.y };
							if (ImGui::InputFloat2("Scale (X, Y)", scale, "%.1f"))
							{
								transform.scale.x = scale[0];
								transform.scale.y = scale[1];

								// Update bounding box if entity has one
								if (e->has<CBoundingBox>())
								{
									auto& bbox = e->get<CBoundingBox>();
									auto spriteSize = e->get<CAnimation>().animation.getSprite().getTexture()->getSize();
									auto animation = e->get<CAnimation>().animation;
									bbox.size = Vec2f(
										spriteSize.x / animation.getFrameCount() * transform.scale.x,
										spriteSize.y * transform.scale.y
									);
									bbox.halfsize = Vec2f(bbox.size.x / 2.0f, bbox.size.y / 2.0f);
								}
							}
						}

						if (e->has<CState>())
						{
							bool isClicked = e->get<CState>().isClicked;
							if (ImGui::Checkbox("Is Clicked", &isClicked))
							{
								e->get<CState>().isClicked = isClicked;
							}
						}

						if (e->has<CAnimation>())
						{
							ImGui::Text("Animation: %s", e->tag().c_str());
							ImGui::Text("Frame Count: %d", e->get<CAnimation>().animation.getFrameCount());
						}

						if (e->has<CBoundingBox>())
						{
							auto& bbox = e->get<CBoundingBox>();
							ImGui::Text("Bounding Box: (%.1f, %.1f)", bbox.size.x, bbox.size.y);
						}

						// Entity operations
						if (ImGui::Button("Select This Entity"))
						{
							// Deselect current entity
							if (m_currentEntity)
							{
								m_currentEntity->get<CState>().isClicked = false;
							}

							// Select this entity
							m_currentEntity = e;
							e->get<CState>().isClicked = true;
						}

						ImGui::SameLine();

						if (ImGui::Button("Delete Entity"))
						{
							if (m_currentEntity == e)
							{
								m_currentEntity = nullptr;
							}
							e->destroy();
							ImGui::TreePop();
							break; // Break out of loop since entity is destroyed
						}

						ImGui::TreePop();
					}
				}

				if (entityCount == 0)
				{
					ImGui::Text("No active entities in the scene.");
				}
			}

			ImGui::Separator();

			// Batch operations
			if (ImGui::Button("Deselect All Entities"))
			{
				for (auto& e : m_entityManager.getEntities())
				{
					e->get<CState>().isClicked = false;
				}
				m_currentEntity = nullptr;
			}

			ImGui::SameLine();

			if (ImGui::Button("Center View on Selected"))
			{
				if (m_currentEntity && m_currentEntity->has<CTransform>())
				{
					auto& pos = m_currentEntity->get<CTransform>().pos;
					sf::View view = m_game.window().getView();
					view.setCenter(pos.x, pos.y);
					m_game.window().setView(view);
				}
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Edit"))
		{
			std::string folderPath = getLevelsDirectory();
			if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
			{
				std::cerr << "FOLDER PATH DOESNT EXIST" << std::endl;
			}

			ImGui::CollapsingHeader("All Created Levels", ImGuiTreeNodeFlags_DefaultOpen);
			{

				for (auto entryPath : std::filesystem::directory_iterator(folderPath))
				{
					std::string pathNoTxt = entryPath.path().string().erase(entryPath.path().string().length() - 4);
					std::string finalString = "Level Name: " + pathNoTxt.erase(0, 58);
					if (ImGui::TreeNode(finalString.c_str()))
					{
						ImGui::Separator();
						int entityCount = 0;
						std::ifstream file(entryPath.path().string());
						std::string str;
						while (file.good())
						{
							file >> str;
							entityCount += 1;
						}
						ImGui::Text("Current Level Entity Count: %d", entityCount);
						if(ImGui::Button("Edit Level"))
						{
							if (m_entityManager.getEntities().empty())
							{
								loadLevel(entryPath.path().string());
							}
							else
							{
								ImGui::OpenPopup("WARNING!!");
							}
						}
						ImVec2 center = ImGui::GetMainViewport()->GetCenter();
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
						if (ImGui::BeginPopupModal("WARNING!!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Warning, you havent saved this level, opening");
							ImGui::Text("another level will delete your progress here.");
							ImGui::Text("Would you like to open anyways?");
							ImGui::Separator();
							if (ImGui::Button("Yes", ImVec2(120, 0)))
							{
								for (auto& entity : m_entityManager.getEntities())
								{
									entity->destroy();
								}
								loadLevel(entryPath.path().string());
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							ImGui::SetItemDefaultFocus();
							if (ImGui::Button("No", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}


						ImGui::SameLine();
						if (ImGui::Button("Delete Level"))
						{
							ImGui::OpenPopup("Delete Confirmation");
						}
						// Always center this window when appearing
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
						if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Are you sure you want to delete this level?");
							ImGui::Text("Deleting means it's gone FOREVER! (A-long time)");
							ImGui::Separator();

							if (ImGui::Button("Yes", ImVec2(120, 0)))
							{
								m_fileToBeDeleted = entryPath.path().string();
								ImGui::CloseCurrentPopup();
							}
							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("No", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}

						ImGui::SameLine();
						if (ImGui::Button("Save Level"))
						{
							ImGui::OpenPopup("Save Confirmation");
						}
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
						if(ImGui::BeginPopupModal("Save Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Are you sure you want to Save this level?");
							ImGui::Text("Saving overrides pervious level layout!");
							ImGui::Separator();
							if (ImGui::Button("Yes", ImVec2(120, 0)))
							{
								saveLevelToFile(pathNoTxt);
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							ImGui::SetItemDefaultFocus();
							if (ImGui::Button("No", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
						
						
						ImGui::Separator();
						ImGui::TreePop();
					}
				}

			}


			ImGui::EndTabItem();

		}
		if (ImGui::BeginTabItem("Save"))
		{
			ImGui::Text("ADD Save Button");

			ImGui::Text("LevelName: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(200.0f);
			ImGui::InputText("##LevelNameInput", m_textBuffer, sizeof(m_textBuffer));

			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				m_newlevelName = m_textBuffer;
			}

			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				//Reset the entire buffer to zeros
				memset(m_textBuffer, 0, sizeof(m_textBuffer));
				m_newlevelName = ""; //Also clear the stored name
			}
			ImGui::Text("Current Level Name: %s", m_newlevelName.c_str());
			ImGui::Separator();

			if (ImGui::Button("Create New Level"))
			{
				if (m_newlevelName != "")
				{
					saveLevelToFile(m_newlevelName);
					ImGui::Text("Created New Level: %s", m_newlevelName.c_str());
				}
			}



			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Scene_Create::drawGrid()
{
	sf::Font gridFont = m_game.assets().getFont("GridFont");
	m_gridText.setFont(gridFont);

	const sf::View& view = m_game.window().getView();
	float width = view.getSize().x;
	float height = view.getSize().y;

	// Calculate visible area bounds
	float leftX = view.getCenter().x - width / 2.f;
	float topY = view.getCenter().y - height / 2.f;
	float rightX = leftX + width;
	float bottomY = topY + height;

	// Adjust starting positions to align with grid
	float nextGridX = leftX - std::fmod(leftX, m_gridSize.x);
	float nextGridY = topY - std::fmod(topY, m_gridSize.y);

	// Create vertex array for grid lines
	sf::VertexArray gridLines(sf::Lines);

	// Draw vertical lines (unchanged)
	for (float x = nextGridX; x < rightX; x += m_gridSize.x)
	{
		gridLines.append(sf::Vertex(sf::Vector2f(x, topY), sf::Color::White));
		gridLines.append(sf::Vertex(sf::Vector2f(x, bottomY), sf::Color::White));
	}

	// Draw horizontal lines (unchanged)
	for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
	{
		gridLines.append(sf::Vertex(sf::Vector2f(leftX, y), sf::Color::White));
		gridLines.append(sf::Vertex(sf::Vector2f(rightX, y), sf::Color::White));
	}

	m_game.window().draw(gridLines);

	// Draw coordinate numbers with (0,0) at bottom left
	m_gridText.setCharacterSize(16);
	m_gridText.setFillColor(sf::Color::White);

	// Calculate how many grid cells fit vertically in the view
	float gridCellsY = height / m_gridSize.y;

	for (float x = nextGridX; x < rightX; x += m_gridSize.x)
	{
		for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
		{
			int cellX = static_cast<int>(x / m_gridSize.x);
			// Calculate cellY by inverting the Y position relative to the bottom
			int cellY = static_cast<int>((bottomY - y) / m_gridSize.y - 1);

			m_gridText.setString("(" + std::to_string(cellX) + "," + std::to_string(cellY) + ")");
			m_gridText.setPosition(x + 2.f, y + 2.f);
			m_game.window().draw(m_gridText);
		}
	}
}

void Scene_Create::initObjectToMouse(const std::string name)
{
	//TODO:: MAKE SURE ALL THE SCALING IS CONCRETE THROUGHOUT ALL ENTITIES
	float scaleX = 2.0f;
	float scaleY = 2.0f;
	if (name == "pointSignUp" || name == "pointSign" || name == "spinIdle")
	{
		scaleX = 3.0f;
		scaleY = 3.0f;

	}
	else if (name == "TallIdleSleep" || name == "ShipIdle")
	{
		scaleX = 4.0f;
		scaleY = 4.0f;
	}
	else if (name == "lvl1BG" || name == "MainMenuBG" || name == "MercuryBG" || name == "VenusBG")
	{
		scaleX = 1;
		scaleY = 1;
	}

	//Create new entity and then add animation and state
	auto newobject = m_entityManager.addEntity(name);
	newobject->add<CAnimation>(m_game.assets().getAnimation(name), true);
	newobject->add<CState>();
	newobject->get<CState>().isClicked = true;

	//Set up bounding boxes
	auto spriteSize = newobject->get<CAnimation>().animation.getSprite().getTexture()->getSize();
	auto& animation = newobject->get<CAnimation>().animation;

	newobject->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));


	// Set up the position of the object based on mouse position
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
	Vec2f worldPos = m_game.window().mapPixelToCoords(mousePos);
	newobject->add<CTransform>(worldPos, Vec2f(0, 0), Vec2f(scaleX, scaleY), 0);

	if (checkTypeOfTile(name) == "MovingPlatform")
	{
		newobject->add<CMovePoints>(3);
		int pixelxDifference = 0;

		for (int i = 0; i < newobject->get<CMovePoints>().movePointCount; i++)
		{
			std::cout << "Created a new point" << "\n";
			auto firstPoint = m_entityManager.addEntity("locationTracker");
			firstPoint->add<CAnimation>(m_game.assets().getAnimation("locationTracker"), false);
			firstPoint->add<CTransform>(Vec2f(newobject->get<CTransform>().pos.x + pixelxDifference, newobject->get<CTransform>().pos.y));
			firstPoint->add<CState>();
			//Set up bounding boxes
			auto spriteSize = firstPoint->get<CAnimation>().animation.getSprite().getTexture()->getSize();
			auto& animation = firstPoint->get<CAnimation>().animation;
			firstPoint->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));

			newobject->get<CMovePoints>().entityPointerVector.push_back(firstPoint->id());

			pixelxDifference += 75;
		}

	}

	//Update the internal scene currentEntity variable
	m_currentEntity = newobject;
}

void Scene_Create::dragCreateInit()
{
	auto newobject = m_entityManager.addEntity(currentPickUp()->tag());
	newobject->add<CAnimation>(m_game.assets().getAnimation(currentPickUp()->tag()), false);
	newobject->add<CState>();

	//Set up bounding boxes
	auto spriteSize = newobject->get<CAnimation>().animation.getSprite().getTexture()->getSize();
	auto animation = newobject->get<CAnimation>().animation;
	newobject->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * currentPickUp()->get<CTransform>().scale.x, spriteSize.y * currentPickUp()->get<CTransform>().scale.y));


	// Set up the position of the object based on mouse position
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
	Vec2f worldPos = m_game.window().mapPixelToCoords(mousePos);
	worldPos.y = m_game.window().getDefaultView().getSize().y - worldPos.y;

	Vec2f gridCords = midPixelToGrid(worldPos.x, worldPos.y);
	newobject->add<CTransform>(gridToMidPixel(gridCords.x, gridCords.y, newobject), Vec2f(0, 0), Vec2f(currentPickUp()->get<CTransform>().scale.x, currentPickUp()->get<CTransform>().scale.y), 0);


}

void Scene_Create::UpdatePosition(std::shared_ptr<Entity> entity)
{
	if (entity->get<CState>().isClicked)
	{
		if (m_snapToGrid)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
			Vec2f worldPos = m_game.window().mapPixelToCoords(mousePos);

			worldPos.y = m_game.window().getDefaultView().getSize().y - worldPos.y;

			Vec2f gridCords = midPixelToGrid(worldPos.x, worldPos.y);

			entity->get<CTransform>().pos = gridToMidPixel(gridCords.x, gridCords.y, entity);
		}
		else if (!m_snapToGrid)
		{
			// Convert mouse position to world coordinates (accounts for view transformation)
			sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
			Vec2f worldPos = m_game.window().mapPixelToCoords(mousePos);

			entity->get<CTransform>().pos = worldPos;
		}
	}
}

Vec2f Scene_Create::midPixelToGrid(float mouseX, float mouseY) const
{
	float gridPosX = std::floor(mouseX / m_gridSize.x);
	float gridPosY = std::floor(mouseY / m_gridSize.y);
	Vec2f returnPos(gridPosX, gridPosY);

	return returnPos;

} 
Vec2f Scene_Create::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float pixelX = gridX * m_gridSize.x;
	float pixelY = gridY * m_gridSize.y;

	// Get the actual scaled size of the entity
	auto& bbox = entity->get<CBoundingBox>();
	float scaledWidth = bbox.size.x;
	float scaledHeight = bbox.size.y;

	if (entity->tag() == "lvl1BG" || entity->tag() == "MainMenuBG" ||  entity->tag() == "MercuryBG" || entity->tag() == "VenusBG")
	{
		Vec2f returnVec = Vec2f(pixelX + entity->get<CAnimation>().animation.getSize().x - scaledWidth, m_game.window().getDefaultView().getSize().y - pixelY - (entity->get<CAnimation>().animation.getSize().y) + scaledHeight);
		return returnVec;
	}
	else
	{
		Vec2f returnVec = Vec2f(pixelX + entity->get<CAnimation>().animation.getSize().x, m_game.window().getDefaultView().getSize().y - pixelY - (entity->get<CAnimation>().animation.getSize().y));
		return returnVec;
	}

}

bool Scene_Create::checkIfEmptyCell(std::shared_ptr<Entity> entity)
{

	for (auto& e : m_entityManager.getEntities())
	{
		if (e == entity) { continue; }

		if (entity->get<CTransform>().pos.x  == e->get<CTransform>().pos.x &&
			entity->get<CTransform>().pos.y  == e->get<CTransform>().pos.y)
		{
			return false;
		}
		//Really meant to be here because of weird scaling issues that happened
		//Bad practice, but atp im done with debugging lol
		else if (
			entity->get<CTransform>().pos.x == e->get<CTransform>().pos.x - 32 &&
			entity->get<CTransform>().pos.y == e->get<CTransform>().pos.y + 32)
		{
			return false;
		}
	}
	return true;
}

void Scene_Create::mouseCollision(std::string actionName)
{
	//Get mouse Position
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
	Vec2f worldPos = m_game.window().mapPixelToCoords(mousePos);
	
	//Iterate through the entities list in reverse because of drawing order
	auto& entities = m_entityManager.getEntities();
	for (auto it = entities.rbegin(); it != entities.rend(); ++it)
	{
		auto e = *it;

		//Get position of the entity so you can do point box collision
		auto topX = e->get<CTransform>().pos.x - e->get<CBoundingBox>().halfsize.x;
		auto topY = e->get<CTransform>().pos.y - e->get<CBoundingBox>().halfsize.y;
		auto bottomX = e->get<CTransform>().pos.x + e->get<CBoundingBox>().halfsize.x;
		auto bottomY = e->get<CTransform>().pos.y + e->get<CBoundingBox>().halfsize.y;

		//If collision is true, pick up the entity you clicked on
		if (worldPos.x < bottomX && worldPos.x > topX && worldPos.y < bottomY && worldPos.y > topY && !m_currentEntity && actionName == "PICKUP")
		{
			m_currentEntity = e;
			e->get<CState>().isClicked = true;
		}
		//Else if you pressed delete, delete the entity
		else if (worldPos.x < bottomX && worldPos.x > topX && worldPos.y < bottomY && worldPos.y > topY && !m_currentEntity && actionName == "DELETE")
		{
			e->destroy();
		}
	}
}

std::shared_ptr<Entity> Scene_Create::currentPickUp()
{
	return m_currentEntity;
}

void Scene_Create::loadLevel(const std::string& filename)
{
	std::ifstream file(filename);
	std::string str;

	while (file >> str)
	{
		if (str == "Tile")
		{

			std::string name;
			float gx, gy;
			float scaleX, scaleY;
			bool repeatAnimation;
			file >> name >> gx >> gy >> scaleX >> scaleY >> repeatAnimation;
			auto tile = m_entityManager.addEntity(name);
			tile->add<CAnimation>(m_game.assets().getAnimation(name), repeatAnimation);
			auto spriteSize = tile->get<CAnimation>().animation.getSprite().getTexture()->getSize();
			auto& animation = tile->get<CAnimation>().animation;
			tile->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));
			tile->add<CTransform>(gridToWorldCoordinates(Vec2f(gx, gy), tile), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);

		}
		else if (str == "DecBG")
		{
			std::string name;
			file >> name;
			auto backGround = m_entityManager.addEntity(name);
			backGround->add<CTransform>();
			backGround->add<CAnimation>(m_game.assets().getAnimation(name), false);
			backGround->add<CState>();
			auto spriteSize = backGround->get<CAnimation>().animation.getSprite().getTexture()->getSize();
			auto& animation = backGround->get<CAnimation>().animation;
			backGround->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount(), spriteSize.y));
		}
		else if (str == "Dec")
		{
			std::string name;
			float gx, gy;
			float scaleX, scaleY;
			bool animate;
			file >> name >> gx >> gy >> scaleX >> scaleY >> animate;
			auto dec = m_entityManager.addEntity(name);
			dec->add<CAnimation>(m_game.assets().getAnimation(name), animate);
			dec->add<CTransform>(gridToWorldCoordinates(Vec2f(gx, gy), dec), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);
			dec->add<CState>();
			//Set up bounding boxes
			auto spriteSize = dec->get<CAnimation>().animation.getSprite().getTexture()->getSize();
			auto& animation = dec->get<CAnimation>().animation;
			dec->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));
		}
		else if (str == "Player")
		{

			float gx, gy;
			float bx, by;
			float scaleX, scaleY;
			float speed, maxspeed, jump, maxJump, hearts;
			file >> gx >> gy >> bx >> by >> scaleX >> scaleY >> speed >> maxspeed >> jump >> maxJump >> hearts;
			auto player = m_entityManager.addEntity("Stand");
			player->add<CAnimation>(m_game.assets().getAnimation("Stand"), true);
			player->add<CState>();
			player->add<CTransform>(gridToWorldCoordinates(Vec2f(gx, gy), player), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);
			player->add<CBoundingBox>(Vec2f(bx * scaleX, by * scaleY));

		}
		else if (str == "Enemy")
		{

			float gx, gy, cx, cy, scaleX, scaleY, vertSpeed, maxSpeed, health;
			std::string enemyName, enemyAnimationName;
			file
				>> gx
				>> gy
				>> cx
				>> cy
				>> scaleX
				>> scaleY
				>> vertSpeed
				>> maxSpeed
				>> health
				>> enemyName
				>> enemyAnimationName;
			auto enemy = m_entityManager.addEntity(enemyAnimationName);
			enemy->add<CAnimation>(m_game.assets().getAnimation(enemyAnimationName), true);
			enemy->add<CState>();
			enemy->add<CTransform>(gridToWorldCoordinates(Vec2f(gx, gy), enemy), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);
			enemy->add<CBoundingBox>(Vec2f(cx * scaleX, cy * scaleY));

		}
		else
		{
			std::cerr << "Unknow tile: " + str + " from level file : " + filename << std::endl;
		}
	}
}

void Scene_Create::onEnd()
{
	m_game.window().setView(sf::View(sf::FloatRect(0, 0,
		m_game.window().getSize().x,
		m_game.window().getSize().y)));
}

Vec2f Scene_Create::worldToGridCoordinates(const Vec2f& worldPos) const
{
	// Use a fixed reference size instead of window size
	const float referenceHeight = 768.0f; // Or your default window height

	float gridX = std::floor(worldPos.x / m_gridSize.x);
	float gridY = std::floor((referenceHeight - worldPos.y) / m_gridSize.y);
	return Vec2f(gridX, gridY);
}

Vec2f Scene_Create::gridToWorldCoordinates(const Vec2f& gridPos, std::shared_ptr<Entity> entity) const
{
	const float referenceHeight = 768.0f;

	float worldX = gridPos.x * m_gridSize.x + entity->get<CBoundingBox>().halfsize.x;
	float worldY = referenceHeight - (gridPos.y * m_gridSize.y + entity->get<CBoundingBox>().halfsize.y);
	return Vec2f(worldX, worldY);
}

std::string Scene_Create::getLevelsDirectory()
{
	char appDataPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);

	std::string levelsPath = std::string(appDataPath) + "\\Galactic_Ink\\createdLevels\\";
	//std::cout << levelsPath << std::endl;
	std::filesystem::create_directories(levelsPath);

	return levelsPath;
}

const std::string Scene_Create::checkTypeOfTile(const std::string& tag)
{

	std::string baseCase = "Tile";
	if (tag.length() > 14)
	{
		std::string lastFourteenChars = tag.substr(tag.length() - 14);
		if (lastFourteenChars == "MovingPlatform")
		{
			return lastFourteenChars;
		}
	}

	if (tag.length() > 8)
	{
		std::string lastEightChars = tag.substr(tag.length() - 8);
		if (lastEightChars == "MoveAble")
		{
			return lastEightChars;
		}
	}
	return baseCase;

}