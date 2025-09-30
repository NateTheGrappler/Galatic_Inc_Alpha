#pragma Once

#include "Scene.h"
#include <map>
#include <memory>
#include "EntityManager.hpp"


class Scene_Menu : public Scene
{
public:
    Scene_Menu(Game& gameEngine);
    void init();
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;
    void setUpEntities();
    void setUpFonts();
    void handleWindowResize();
    void updateCreatedLevelsDisplay();
    std::string getLevelsDirectory();
    void sAnimation();

private:


    std::map<int, sf::Text>                m_menuTexts;
    std::map<int, sf::Text>                m_menuTextsCreated;
    std::vector<std::string>               m_levelPaths;
    std::vector<std::string>               m_levelPathsCreated;
    std::vector<std::string>               m_levelNames;
    std::vector<std::string>               m_levelNamesCreated;
    std::vector<sf::RectangleShape>        m_menuBoxes;

    std::vector<sf::RectangleShape>        m_builtInLevels;
    std::vector<sf::RectangleShape>        m_CreatedLevels;

    int                                    m_selectedMenuIndex = 0;
    int                                    m_currentCreatedPage = 0;
    int                                    m_currentTab = 0;
    int                                    m_selectedCreatedIndex = 0;

    sf::Text                               m_levelCreateText;

};