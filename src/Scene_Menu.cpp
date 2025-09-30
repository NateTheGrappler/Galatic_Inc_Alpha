// Scene_Menu.cpp
#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Scene_Create.h"
#include "Assets.hpp"
#include "GameEngine.h"
#include "Components.hpp"
#include "Action.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map> 
#include <shlobj.h>

Scene_Menu::Scene_Menu(Game& gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Enter, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::C, "CREATE");
    registerAction(sf::Event::Resized, "RESIZED");
    registerMouseAction(sf::Mouse::Left, "CLICK");
    registerAction(sf::Keyboard::Q, "LEFT");
    registerAction(sf::Keyboard::E, "RIGHT");
    registerAction(sf::Keyboard::A, "SWITCHTAB");
    registerAction(sf::Keyboard::D, "SWITCHTAB");
    registerAction(sf::Keyboard::Left, "SWITCHTAB");
    registerAction(sf::Keyboard::Right, "SWITCHTAB");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");


    // Clear existing paths
    m_levelPaths.clear();
    m_levelNames.clear();
    m_levelPathsCreated.clear();
    m_levelNamesCreated.clear();

    // Path to your created levels directory
    std::string levelsDir = getLevelsDirectory();
    if (std::filesystem::exists(levelsDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(levelsDir))
        {
            if (entry.is_regular_file())
            {
                m_levelPathsCreated.push_back(entry.path().string());

                // Extract just the filename without extension
                std::string filename = entry.path().filename().string();
                std::string levelName = filename.substr(0, filename.find_last_of('.'));
                m_levelNamesCreated.push_back(levelName);
            }
        }
    }

    // Path to built-in levels directory
    std::string levelsDir2 = std::string(RESOURCES_PATH) + "levels/";
    std::vector<std::filesystem::path> levelPaths;

    if (std::filesystem::exists(levelsDir2)) {
        for (const auto& entry : std::filesystem::directory_iterator(levelsDir2))
        {
            if (entry.is_regular_file()) {
                levelPaths.push_back(entry.path());
            }
        }
    }

    // Define the solar system order
    std::vector<std::string> solarSystemOrder = {
        "Mercury", "Venus", "Earth", "Mars", "Jupiter",
        "Saturn", "Uranus", "Neptune"
    };

    // Sort files according to solar system order
    std::sort(levelPaths.begin(), levelPaths.end(),
        [&solarSystemOrder](const std::filesystem::path& a, const std::filesystem::path& b)
        {
            std::string filenameA = a.stem().string();
            std::string filenameB = b.stem().string();

            auto itA = std::find(solarSystemOrder.begin(), solarSystemOrder.end(), filenameA);
            auto itB = std::find(solarSystemOrder.begin(), solarSystemOrder.end(), filenameB);

            if (itA != solarSystemOrder.end() && itB != solarSystemOrder.end()) {
                return std::distance(solarSystemOrder.begin(), itA) <
                    std::distance(solarSystemOrder.begin(), itB);
            }
            else if (itA != solarSystemOrder.end()) {
                return true;
            }
            else if (itB != solarSystemOrder.end()) {
                return false;
            }
            return filenameA < filenameB;
        });

    // Process built-in levels
    for (const auto& path : levelPaths)
    {
        m_levelPaths.push_back(path.string());
        std::string levelName = path.stem().string();
        m_levelNames.push_back(levelName);
    }

    // Initialize selection rectangles
    m_builtInLevels.clear();
    m_CreatedLevels.clear();

    // Create rectangles for built-in levels
    for (size_t i = 0; i < m_levelPaths.size(); i++) {
        sf::RectangleShape rec;
        rec.setFillColor(sf::Color::Transparent);
        rec.setOutlineColor(sf::Color(4, 44, 98, 255));
        rec.setOutlineThickness(2);
        m_builtInLevels.push_back(rec);
    }

    // Create only 15 rectangles for created levels (limited display)
    for (int i = 0; i < 11; i++) {
        sf::RectangleShape rec;
        rec.setFillColor(sf::Color::Transparent);
        rec.setOutlineColor(sf::Color(4, 44, 98, 255));
        rec.setOutlineThickness(2);
        m_CreatedLevels.push_back(rec);
    }

    if (!m_builtInLevels.empty()) {
        m_builtInLevels[0].setFillColor(sf::Color(255, 255, 255, 64));
    }

    m_currentCreatedPage = 0;
    m_selectedCreatedIndex = 0;

    setUpEntities();
    setUpFonts();
}

void Scene_Menu::setUpFonts()
{
    m_menuTexts.clear();
    m_menuTextsCreated.clear();

    // Set up built-in levels text
    if (!m_menuBoxes.empty() && !m_levelPaths.empty()) 
    {
        sf::Vector2f boxPos = m_menuBoxes[0].getPosition();
        sf::Vector2f boxSize = m_menuBoxes[0].getSize();
        float slotHeight = boxSize.y / m_levelPaths.size();

        for (size_t i = 0; i < m_levelPaths.size(); i++) 
        {
            std::string lvlName = m_levelNames[i];

            sf::Text text;
            text.setFont(m_game.assets().getFont("mainMenuFont"));
            text.setString(lvlName);
            text.setLetterSpacing(1);
            text.setStyle(sf::Text::Bold);
            text.setCharacterSize(24); // Smaller font for better fit
            text.setFillColor(sf::Color(145, 240, 134));

            // Center text in its slot
            sf::FloatRect textBounds = text.getLocalBounds();
            float xPos = boxPos.x + (boxSize.x - textBounds.width) / 2.0f;
            float yPos = boxPos.y + (i * slotHeight) + (slotHeight - textBounds.height) / 2.0f;

            text.setPosition(xPos, yPos);
            m_menuTexts[i] = text;
        }
    }

    // Set up created levels text with pagination
    if (m_menuBoxes.size() > 1 && !m_levelNamesCreated.empty()) 
    {
        updateCreatedLevelsDisplay();
    }
    sf::Vector2f boxPos = m_menuBoxes[1].getPosition();
    sf::Vector2f boxSize = m_menuBoxes[1].getSize();

    sf::Text text;
    text.setFont(m_game.assets().getFont("mainMenuFont"));
    text.setString("Press C To Create Level");
    text.setLetterSpacing(1);
    text.setStyle(sf::Text::Bold);
    text.setCharacterSize(24); // Smaller font for better fit
    text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = text.getLocalBounds();
    float xPos = boxPos.x + (boxSize.x - textBounds.width) / 2.0f;
    float yPos = 75 / 2.0f;

    text.setPosition(xPos, yPos);
    m_levelCreateText = text;
}

void Scene_Menu::updateCreatedLevelsDisplay()
{
    m_menuTextsCreated.clear();

    sf::Vector2f boxPos = m_menuBoxes[1].getPosition();
    sf::Vector2f boxSize = m_menuBoxes[1].getSize();
    float slotHeight = boxSize.y / 11; // Fixed 11 slots

    int startIndex = m_currentCreatedPage * 11;
    int endIndex = std::min(startIndex + 11, static_cast<int>(m_levelNamesCreated.size()));

    for (int i = startIndex; i < endIndex; i++) 
    {
        std::string lvlName = m_levelNamesCreated[i];
        if (lvlName.length() > 20) 
        { // Truncate long names
            lvlName = lvlName.substr(0, 17) + "...";
        }

        sf::Text text;
        text.setFont(m_game.assets().getFont("mainMenuFont"));
        text.setString(lvlName);
        text.setLetterSpacing(1);
        text.setStyle(sf::Text::Bold);
        text.setCharacterSize(20); // Smaller font for created levels
        text.setFillColor(sf::Color(145, 240, 134));

        // Center text in its slot
        sf::FloatRect textBounds = text.getLocalBounds();
        float xPos = boxPos.x + (boxSize.x - textBounds.width) / 2.0f;
        float yPos = boxPos.y + 69 + ((i - startIndex) * slotHeight) + (slotHeight - textBounds.height) / 2.0f;

        text.setPosition(xPos, yPos);
        m_menuTextsCreated[i - startIndex] = text;
    }
}

void Scene_Menu::update()
{
    m_entityManager.update();
    m_game.sUserInput();
    sAnimation();
    sRender();
}

void Scene_Menu::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_currentTab == 0) 
            { // Built-in levels tab
                m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color::Transparent);
                m_selectedMenuIndex = (m_selectedMenuIndex - 1 + m_menuTexts.size()) % m_menuTexts.size();
                m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color(255, 255, 255, 64));
            }
            else 
            { // Created levels tab
                if (!m_menuTextsCreated.empty()) 
                {
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color::Transparent);
                    m_selectedCreatedIndex = (m_selectedCreatedIndex - 1 + m_menuTextsCreated.size()) % m_menuTextsCreated.size();
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color(255, 255, 255, 64));
                }
            }
        }
        else if (action.name() == "DOWN")
        {
            if (m_currentTab == 0) 
            {
                m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color::Transparent);
                m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuTexts.size();
                m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color(255, 255, 255, 64));
            }
            else 
            {
                if (!m_menuTextsCreated.empty()) 
                {
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color::Transparent);
                    m_selectedCreatedIndex = (m_selectedCreatedIndex + 1) % m_menuTextsCreated.size();
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color(255, 255, 255, 64));
                }
            }
        }
        else if (action.name() == "SWITCHTAB")
        {
             if (m_currentTab == 0)
             {
                m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color(255, 255, 255, 64));
                m_currentTab = 1;
                m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color::Transparent);

             }
             else
             {
                 m_builtInLevels[m_selectedMenuIndex].setFillColor(sf::Color(255, 255, 255, 64));
                 m_currentTab = 0;
                 m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color::Transparent);
             }
        }
        else if (action.name() == "LEFT")
        {
            if (m_currentTab == 1 && m_currentCreatedPage > 0) 
            {
                m_currentCreatedPage--;
                updateCreatedLevelsDisplay();
                m_selectedCreatedIndex = 0;
                if (!m_menuTextsCreated.empty()) 
                {
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color(255, 255, 255, 64));
                }
            }
        }
        else if (action.name() == "RIGHT")
        {
            if (m_currentTab == 1 && (m_currentCreatedPage + 1) * 15 < m_levelNamesCreated.size()) 
            {
                m_currentCreatedPage++;
                updateCreatedLevelsDisplay();
                m_selectedCreatedIndex = 0;
                if (!m_menuTextsCreated.empty()) 
                {
                    m_CreatedLevels[m_selectedCreatedIndex].setFillColor(sf::Color(255, 255, 255, 64));
                }
            }
        }
        else if (action.name() == "PLAY")
        {
            if (m_currentTab == 0) 
            {
                m_game.changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]), true);
            }
            else 
            {
                int actualIndex = m_currentCreatedPage * 15 + m_selectedCreatedIndex;
                if (actualIndex < m_levelPathsCreated.size()) 
                {
                    m_game.changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPathsCreated[actualIndex]), true);
                }
            }
        }
        else if (action.name() == "CREATE")
        {
            m_game.changeScene("CREATE", std::make_shared<Scene_Create>(m_game), true);
        }
        else if (action.name() == "CLICK")
        {
            sf::Vector2f worldPos = m_game.window().mapPixelToCoords(sf::Mouse::getPosition(m_game.window()));
            // Handle tab switching via click if desired
        }
        else if (action.name() == "RESIZED")
        {
            handleWindowResize();
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}


void Scene_Menu::sAnimation()
{
    //This updates every single one of the animations, moving it to the next frame or ending it
    for (auto& e : m_entityManager.getEntities())
    {
        if (!e->has<CAnimation>()) { continue; }

        if (e->has<CAnimation>() && e->get<CAnimation>().repeat)
        {
            e->get<CAnimation>().animation.update();
        }
        else if (!e->get<CAnimation>().animation.hasEnded())
        {
            e->get<CAnimation>().animation.update();
        }

        auto& animation = e->get<CAnimation>();
        auto& name = e->tag();
        auto& currentAnim = animation.animation.getName();

        if (name == "Ship" && currentAnim == "ShipIdleLarge" && animation.animation.hasEnded())
        {
            e->add<CAnimation>(m_game.assets().getAnimation("ShipBeamDownL"), false);
        }
        else if (name == "Ship" && currentAnim == "ShipBeamDownL" && animation.animation.hasEnded())
        {
            e->add<CAnimation>(m_game.assets().getAnimation("ShipIdleLarge"), false);
        }
    }
}

void Scene_Menu::sRender()
{
    m_game.window().clear(sf::Color(100, 100, 255));
    handleWindowResize();

    // Draw entities
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() == "MainMenuBG")
        {
            // Get window and view dimensions
            sf::Vector2u windowSize = m_game.window().getSize();
            sf::FloatRect viewport = m_game.window().getView().getViewport();

            auto& transform = e->get<CTransform>();
            auto& animation = e->get<CAnimation>();

            // Center the background
            transform.pos.x = m_game.window().getView().getCenter().x;
            transform.pos.y = m_game.window().getView().getCenter().y;

            // Get the original size of the sprite/texture
            sf::FloatRect spriteBounds = animation.animation.getSprite().getLocalBounds();

            // Calculate scale factors for both axes
            float scaleX = windowSize.x / spriteBounds.width;
            float scaleY = windowSize.y / spriteBounds.height;

            // Use the larger scale factor to ensure full coverage
            float scale = std::max(scaleX, scaleY);

            // Apply the uniform scale
            transform.scale.x = scale;
            transform.scale.y = scale;
        }
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

    // Draw menu boxes
    for (auto& rec : m_menuBoxes)
    {
        m_game.window().draw(rec);
    }

    m_game.window().draw(m_entityManager.getEntities("Ship")[0]->get<CAnimation>().animation.getSprite());

    // Draw built-in levels
    float heightOfRecs = 0;
    for (size_t i = 0; i < m_builtInLevels.size(); i++)
    {
        m_builtInLevels[i].setSize({ m_menuBoxes[0].getSize().x, m_menuBoxes[0].getSize().y / m_levelPaths.size() });
        m_builtInLevels[i].setPosition(m_menuBoxes[0].getPosition().x, m_menuBoxes[0].getPosition().y + heightOfRecs);
        heightOfRecs += m_menuBoxes[0].getSize().y / m_levelPaths.size();
        m_game.window().draw(m_builtInLevels[i]);
    }

    // Draw created levels (only visible ones with pagination)
    float heightOfRecs2 = 0;
    int startIndex = m_currentCreatedPage * 11;
    int endIndex = std::min(startIndex + 11, static_cast<int>(m_levelNamesCreated.size()));

    for (int i = 0; i < endIndex - startIndex; i++)
    {
        m_CreatedLevels[i].setSize({ m_menuBoxes[1].getSize().x, m_menuBoxes[1].getSize().y / 11 });
        m_CreatedLevels[i].setPosition(m_menuBoxes[1].getPosition().x, m_menuBoxes[1].getPosition().y + 75 + heightOfRecs2);
        heightOfRecs2 += m_menuBoxes[1].getSize().y / 11;
        m_game.window().draw(m_CreatedLevels[i]);
    }

    // Draw page indicator for created levels
    if (m_levelNamesCreated.size() > 12) 
    {
        sf::Text pageText;
        pageText.setFont(m_game.assets().getFont("mainMenuFont"));
        pageText.setString("Page " + std::to_string(m_currentCreatedPage + 1) +
            "/" + std::to_string((m_levelNamesCreated.size() + 14) / 11));
        pageText.setCharacterSize(16);
        pageText.setFillColor(sf::Color::White);
        pageText.setPosition(m_menuBoxes[1].getPosition().x + 10, m_menuBoxes[1].getPosition().y + m_menuBoxes[1].getSize().y + 10);
        m_game.window().draw(pageText);
    }

    // Draw all text
    for (const auto& pair : m_menuTexts) 
    {
        m_game.window().draw(pair.second);
    }
    for (const auto& pair : m_menuTextsCreated) 
    {
        m_game.window().draw(pair.second);
    }
    m_game.window().draw(m_levelCreateText);

    m_game.window().display();
}

void Scene_Menu::setUpEntities()
{
    auto backGround = m_entityManager.addEntity("MainMenuBG");
    backGround->add<CTransform>();
    backGround->add<CAnimation>(m_game.assets().getAnimation("MainMenuBG"), false);

    auto Title = m_entityManager.addEntity("Title");
    Title->add<CTransform>(Vec2f(192.0f, 128.0f), Vec2f(0.0f, 0.0f), Vec2f(1.5f, 1.5f), 0);
    Title->add<CAnimation>(m_game.assets().getAnimation("Title"), false);
    
    sf::RectangleShape Rec;
    Rec.setSize(sf::Vector2f(341, 462));
    Rec.setPosition(35, 265);
    Rec.setFillColor(sf::Color(0, 0, 0, 150));
    Rec.setOutlineColor(sf::Color(4, 44, 98, 255));
    Rec.setOutlineThickness(6);
    m_menuBoxes.push_back(Rec);

    sf::RectangleShape Rec2;
    Rec2.setSize(sf::Vector2f(377, 689));
    Rec2.setPosition(431, 39);
    Rec2.setFillColor(sf::Color(0, 0, 0, 150));
    Rec2.setOutlineColor(sf::Color(4, 44, 98, 255));
    Rec2.setOutlineThickness(6);
    m_menuBoxes.push_back(Rec2);

    sf::RectangleShape Rec3;
    Rec3.setSize(sf::Vector2f(377, 689));
    Rec3.setPosition(861, 39);
    Rec3.setFillColor(sf::Color(0, 0, 0, 150));
    Rec3.setOutlineColor(sf::Color(4, 44, 98, 255));
    Rec3.setOutlineThickness(6);
    m_menuBoxes.push_back(Rec3);

    auto spaceShip = m_entityManager.addEntity("Ship");
    spaceShip->add<CTransform>(Vec2f(1049.5, 320), Vec2f(0.0f, 0.0f), Vec2f(1, 1), 0);
    spaceShip->add<CAnimation>(m_game.assets().getAnimation("ShipIdleLarge"), false);
    
}

void Scene_Menu::handleWindowResize()
{
    // Get the original/default view size
    sf::Vector2f originalViewSize = m_game.window().getDefaultView().getSize();

    // Get the current window size
    sf::Vector2f currentWindowSize = static_cast<sf::Vector2f>(m_game.window().getSize());

    // Calculate scale factors
    float scaleFactorX = currentWindowSize.x / originalViewSize.x;
    float scaleFactorY = currentWindowSize.y / originalViewSize.y;

    // Update the view to center on the window
    sf::View mainView = m_game.window().getView();
    mainView.setSize(currentWindowSize);
    mainView.setCenter(currentWindowSize.x / 2.0f, currentWindowSize.y / 2.0f);
    m_game.window().setView(mainView);

    // Reset all transforms to their original state first
    for (auto& e : m_entityManager.getEntities())
    {
        auto& transform = e->get<CTransform>();
        auto& animation = e->get<CAnimation>();

        // Get original scale and apply new scaling
        auto& originalScale = transform.ogScale;
        transform.scale.x = originalScale.x * scaleFactorX;
        transform.scale.y = originalScale.y * scaleFactorY;

        // Calculate the scaled size difference for proper positioning
        float originalWidth = animation.animation.getSize().x * originalScale.x;
        float scaledWidth = animation.animation.getSize().x * transform.scale.x;
        float widthDifference = scaledWidth - originalWidth;

        // Reposition based on the original position and scaling
        // This maintains the relative position from the edges
        transform.pos.x = transform.ogPos.x * scaleFactorX;
        transform.pos.y = transform.ogPos.y * scaleFactorY;

        transform.pos.x += (currentWindowSize.x - originalViewSize.x * scaleFactorX) / 2.0f;
        transform.pos.y += (currentWindowSize.y - originalViewSize.y * scaleFactorY) / 2.0f;
    }

    // Scale menu boxes (assuming they're sf::RectangleShape or similar)
    static std::unordered_map<sf::RectangleShape*, sf::Vector2f> originalSizes;
    static std::unordered_map<sf::RectangleShape*, sf::Vector2f> originalPositions;

    for (auto& rec : m_menuBoxes)
    {
        // Store originals once
        if (originalSizes.find(&rec) == originalSizes.end())
        {
            originalSizes[&rec] = rec.getSize();
            originalPositions[&rec] = rec.getPosition();
        }

        // Always reset to original before applying new scale
        rec.setSize(originalSizes[&rec]);
        rec.setPosition(originalPositions[&rec]);

        // Then apply current scaling
        rec.setSize(sf::Vector2f(
            originalSizes[&rec].x * scaleFactorX,
            originalSizes[&rec].y * scaleFactorY
        ));

        rec.setPosition(sf::Vector2f(
            originalPositions[&rec].x * scaleFactorX,
            originalPositions[&rec].y * scaleFactorY
        ));
    }

    // Recalculate text positions and sizes after resize
    if (!m_menuBoxes.empty()) 
    {
        sf::Vector2f boxPos = m_menuBoxes[0].getPosition();
        sf::Vector2f boxSize = m_menuBoxes[0].getSize();
        float slotHeight = boxSize.y / m_levelPaths.size();

        for (auto& pair : m_menuTexts) 
        {
            size_t i = pair.first;
            sf::Text& text = pair.second;

            // Recalculate position
            float yPos = boxPos.y + (i * slotHeight) + (slotHeight / 2);

            // Re-center text
            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                textBounds.top + textBounds.height / 2.0f);
            text.setPosition(boxPos.x + (boxSize.x / 2.0f), yPos);
        }
        updateCreatedLevelsDisplay();

    }
    sf::Vector2f boxPos = m_menuBoxes[1].getPosition();
    sf::Vector2f boxSize = m_menuBoxes[1].getSize();

    sf::Text text;
    text.setFont(m_game.assets().getFont("mainMenuFont"));
    text.setString("Press C To Create Level");
    text.setLetterSpacing(1);
    text.setStyle(sf::Text::Bold);
    text.setCharacterSize(32 * scaleFactorY); // Smaller font for better fit
    text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = text.getLocalBounds();
    float xPos = boxPos.x + (boxSize.x - textBounds.width) / 2.0f + scaleFactorY;
    float yPos = 75 / 2.0f;

    text.setPosition(xPos, yPos);
    m_levelCreateText = text;


}

void Scene_Menu::onEnd() 
{
    m_game.quit();
}

std::string Scene_Menu::getLevelsDirectory()
{
    char appDataPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);

    std::string levelsPath = std::string(appDataPath) + "\\Galactic_Ink\\createdLevels\\";
    //std::cout << levelsPath << std::endl;
    std::filesystem::create_directories(levelsPath);

    return levelsPath;
}