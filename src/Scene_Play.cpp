#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "Physics.hpp"
#include "Assets.hpp"
#include "GameEngine.h"
#include "Components.hpp"
#include "Action.hpp"
#include "Utilis.hpp"
#include <algorithm>
#include <cmath>


Scene_Play::Scene_Play(Game& gameEngine, const std::string levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	std::cout << levelPath << std::endl;
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P,         "PAUSE");
	registerAction(sf::Keyboard::Escape,    "QUIT");
	registerAction(sf::Keyboard::BackSpace, "ESCAPE");
	registerAction(sf::Keyboard::T,         "TOGGLE_TEXTURE");      //Toggle drawing (T)exture
	registerAction(sf::Keyboard::C,         "TOGGLE_COLLISION");    //Toggle drawing (C)ollision
	registerAction(sf::Keyboard::G,         "TOGGLE_GRID");         //Toggle drawing (G)rid
	registerAction(sf::Keyboard::R,         "TOGGLE_RAYCAST");
	registerAction(sf::Keyboard::W,         "JUMP");
	registerAction(sf::Keyboard::A,         "LEFT");
	registerAction(sf::Keyboard::D,         "RIGHT");
	registerAction(sf::Keyboard::LShift,    "RUN");
	registerAction(sf::Keyboard::F1,        "FPS");
	registerMouseAction(sf::Mouse::Left,    "ATTACK");
	registerMouseAction(sf::Mouse::Right,   "SHOOT");
	//TODO: Register all gameplay Actions

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game.assets().getFont("GridFont"));

	loadLevel(levelPath);
}

Vec2f Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float pixelX = gridX * m_gridSize.x;
	float pixelY = gridY * m_gridSize.y;
	if (entity->has<CText>())
	{
		Vec2f returnTextVec = Vec2f(pixelX -32, m_game.window().getDefaultView().getSize().y - pixelY );
		return returnTextVec;
	}
	if (entity->tag() == "Player")
	{
		Vec2f returnVec2 = Vec2f(pixelX + entity->get<CAnimation>().animation.getSize().x / m_playerConfig.SCALEX, m_game.window().getDefaultView().getSize().y - pixelY - entity->get<CAnimation>().animation.getSize().y / m_playerConfig.SCALEY);
		return returnVec2;
	}
	Vec2f returnVec = Vec2f(pixelX + entity->get<CAnimation>().animation.getSize().x / entity->get<CTransform>().scale.x, m_game.window().getDefaultView().getSize().y - pixelY - entity->get<CAnimation>().animation.getSize().y / entity->get<CTransform>().scale.y);
	return returnVec;
}

void Scene_Play::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();
	std::ifstream file(filename);
	std::string str;
	if (!file.is_open())
	{
		std::cerr << "Unable to open level file: " + filename << std::endl;
	}
	while (file.good())
	{
		file >> str;
		if (str == "Tile")
		{
			std::string name;
			float gx, gy;
			float scaleX, scaleY;
			bool repeatAnimation;
			file >> name >> gx >> gy >> scaleX >> scaleY >> repeatAnimation;
			auto tile = m_entityManager.addEntity(name);
			tile->add<CAnimation>(m_game.assets().getAnimation(name), repeatAnimation);
			tile->add<CTransform>(gridToMidPixel(gx, gy, tile), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);
			auto spriteSize = tile->get<CAnimation>().animation.getSprite().getTexture()->getSize();
			auto animation = tile->get<CAnimation>().animation;
			tile->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));
		}
		else if (str == "DecBG")
		{
			std::string name;
			file >> name;
			auto backGround = m_entityManager.addEntity(name);
			backGround->add<CTransform>();
			backGround->add<CAnimation>(m_game.assets().getAnimation(name), true);
		}
		else if (str == "Text")
		{
			std::string fontName, readInString;
			float gx, gy, textSize, r, g, b;
			file >> fontName >> readInString >> gx >> gy >> r >> g >> b >> textSize;
			auto textEntity = m_entityManager.addEntity(str);
			textEntity->add<CText>(m_game.assets().getFont(fontName));
			textEntity->add<CTransform>(gridToMidPixel(gx, gy, textEntity), Vec2f(0, 0), Vec2f(0, 0), 0);
			auto& text = textEntity->get<CText>().text;
			text.setString(readInString);
			text.setCharacterSize(textSize);
			text.setFillColor(sf::Color(r, g, b));

		}
		else if (str == "Dec")
		{
			std::string name;
			float gx, gy;
			float scaleX, scaleY;
			bool animate;
			file >> name >> gx >> gy >> scaleX >> scaleY >> animate;
			auto dec = m_entityManager.addEntity(name);
			dec->add<CAnimation>(m_game.assets().getAnimation(name), true);
			dec->add<CTransform>(gridToMidPixel(gx, gy, dec), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);

			//The caviate of needing somewhere to init the "flag" of the game
			if (name == "ShipIdle")
			{
				dec->add<CTransform>(gridToMidPixel(gx, gy - 1, dec), Vec2f(0.0f, 0.0f), Vec2f(scaleX, scaleY), 0);
				dec->add<CState>();
				dec->add<CRayCast>(Vec2f(dec->get<CTransform>().pos.x, dec->get<CTransform>().pos.y), 4, 0, -64);
				auto spriteSize = dec->get<CAnimation>().animation.getSprite().getTexture()->getSize();
				auto& animation = dec->get<CAnimation>().animation;
				dec->add<CBoundingBox>(Vec2f(spriteSize.x / animation.getFrameCount() * scaleX, spriteSize.y * scaleY));
			}
		}
		else if (str == "Player")
		{
			file
				>> m_playerConfig.X
				>> m_playerConfig.Y
				>> m_playerConfig.CX
				>> m_playerConfig.CY
				>> m_playerConfig.SCALEX
				>> m_playerConfig.SCALEY
				>> m_playerConfig.SPEED
				>> m_playerConfig.MAXSPEED
				>> m_playerConfig.JUMP
				>> m_playerConfig.GRAVITY
				>> m_playerConfig.HEARTS;
			spawnPlayer();
		}
		else if (str == "Enemy")
		{
			file
				>> m_enemyConfig.GX
				>> m_enemyConfig.GY
				>> m_enemyConfig.CX
				>> m_enemyConfig.CY
				>> m_enemyConfig.SCALEX
				>> m_enemyConfig.SCALEY
				>> m_enemyConfig.VERTSPEED
				>> m_enemyConfig.MAXSPEED
				>> m_enemyConfig.Health
				>> m_enemyConfig.enemyName
				>> m_enemyConfig.enemyAnimationName;
			spawnEnemy();
		}
		//else if (str == "UI")
		//{
		//	std::string name, animationName;
		//	float gx, gy, scaleX, scaleY;
		//	file >> name >> gx >> gy >> scaleX >> scaleY >> animationName;
		//	auto uiElement = m_entityManager.addEntity(name);
		//	uiElement->add<CUserInterface>(true);
		//	uiElement->add<CState>();
		//	uiElement->add<CAnimation>(m_game.assets().getAnimation(animationName), true);
		//	uiElement->add<CTransform>(gridToMidPixel(gx, gy, uiElement), Vec2f(0, 0), Vec2f(scaleX, scaleY), 0);
		//}
		else
		{
			std::cerr << "Unknow tile: " + str +  " from level file : " + filename << std::endl;
		}
	}
}

void Scene_Play::spawnPlayer()
{
	auto player = m_entityManager.addEntity("Player");
	player->add<CAnimation>(m_game.assets().getAnimation("Stand"), true);
	player->add<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, player), Vec2f(0, 10), Vec2f(m_playerConfig.SCALEX, m_playerConfig.SCALEY), 0);
	player->add<CBoundingBox>(Vec2f(m_playerConfig.CX, m_playerConfig.CY));
	player->add<CGravity>(m_playerConfig.GRAVITY);
	player->add<CState>(States::stand);
	player->add<CInput>();
	player->add<CRayCast>(Vec2f(player->get<CTransform>().pos.x, player->get<CTransform>().pos.y), 4);
	player->add<CHealth>(5);

	auto& transform = player->get<CTransform>().pos;
	auto& bbox = player->get<CBoundingBox>();

	for (int i = 0; i < m_playerConfig.HEARTS; i++)
	{
		auto uiElement = m_entityManager.addEntity("playerHeart");
		uiElement->add<CUserInterface>(true);
		uiElement->add<CState>();
		uiElement->add<CBoundingBox>(Vec2f(0, 0));
		uiElement->add<CAnimation>(m_game.assets().getAnimation("tentaIdle"), true);
		uiElement->add<CTransform>(gridToMidPixel(i, 0, uiElement), Vec2f(0, 0), Vec2f(1, 1), 0);
	}
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity, std::string& idetifier)
{
	for (auto& attackBox : m_entityManager.getEntities("attackBox"))
	{
		attackBox->destroy();
	}
	if (idetifier == "Hori")
	{
		auto& transform = entity->get<CTransform>();
		float oneScaleX = transform.scale.x / std::abs(transform.scale.x);
		float speedUpfactor = 1.25;
		//TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
		auto bullet = m_entityManager.addEntity("BulletHori");
		bullet->add<CState>().state = States::flying;
		bullet->add<CTransform>(Vec2f(transform.pos.x, transform.pos.y), Vec2f(-m_playerConfig.SPEED * oneScaleX * speedUpfactor, 0), Vec2f(-2.0f * oneScaleX, 2.0f), 0);
		bullet->add<CAnimation>(m_game.assets().getAnimation("InkBallFlyH"), true);
		auto& animation = bullet->get<CAnimation>().animation;
		auto& bulletTransform = bullet->get<CTransform>();
		bullet->add<CBoundingBox>(Vec2f(std::abs(animation.getSprite().getTexture()->getSize().x * bulletTransform.scale.x / animation.getFrameCount()), bullet->get<CAnimation>().animation.getSprite().getTexture()->getSize().y));

	}
	else if (idetifier == "Verti")
	{
		auto& transform = entity->get<CTransform>();
		float oneScaleY = transform.scale.y / std::abs(transform.scale.y);
		float speedUpfactor = 1.25;
		//TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
		auto bullet = m_entityManager.addEntity("BulletVerti");
		bullet->add<CState>().state = States::flying;
		bullet->add<CTransform>(Vec2f(transform.pos.x, transform.pos.y), Vec2f(0, m_playerConfig.SPEED * oneScaleY * speedUpfactor), Vec2f(2.0f, 2.0f * oneScaleY), 0);
		bullet->add<CAnimation>(m_game.assets().getAnimation("InkBallFlyV"), true);
		auto& animation = bullet->get<CAnimation>().animation;
		auto& bulletTransform = bullet->get<CTransform>();
		bullet->add<CBoundingBox>(Vec2f(animation.getSprite().getTexture()->getSize().y, animation.getSprite().getTexture()->getSize().x * bulletTransform.scale.x / animation.getFrameCount()));

	}
}

void Scene_Play::spawnEnemy()
{
	auto enemy = m_entityManager.addEntity(m_enemyConfig.enemyName);
	enemy->add<CAnimation>(m_game.assets().getAnimation(m_enemyConfig.enemyAnimationName), true);
	enemy->add<CTransform>(gridToMidPixel(m_enemyConfig.GX, m_enemyConfig.GY -1 , enemy), Vec2f(0, 0), Vec2f(m_enemyConfig.SCALEX, m_enemyConfig.SCALEY), 0);
	enemy->add<CBoundingBox>(Vec2f(m_enemyConfig.CX * m_enemyConfig.SCALEX / 2, m_enemyConfig.CY * m_enemyConfig.SCALEY));
	enemy->add<CHealth>(100.0f);
	enemy->add<CState>(States::idle, true);
	enemy->get<CState>().hostileState = "hostile";
	if (m_enemyConfig.enemyName == "spinShroom")
	{
		enemy->add<CRayCast>(Vec2f(enemy->get<CTransform>().pos.x, enemy->get<CTransform>().pos.y), 4, 0, 32);
	}
}

void Scene_Play::spawnAttack(std::shared_ptr<Entity> entity)
{
	auto& pTransform = entity->get<CTransform>();
	auto& pBBox = entity->get<CBoundingBox>();
	float oneScaleX = -pTransform.scale.x / std::abs(pTransform.scale.x);

	auto attackBox = m_entityManager.addEntity("attackBox");
	if (m_entityManager.getEntities("attackBox").size() > 1)
	{
		m_entityManager.getEntities("attackBox").back()->destroy();
	}
	attackBox->add<CTransform>(Vec2f(pTransform.pos.x + pBBox.halfsize.x * oneScaleX, pTransform.pos.y), Vec2f(0, 0), Vec2f(0, 0), 0);
	attackBox->add<CBoundingBox>(Vec2f(m_playerConfig.CX, m_playerConfig.CY));
	attackBox->add<CLifespan>(60, 0);
}

void Scene_Play::update()
{
	m_entityManager.update(); //Update entities so handle entity destruction

	if (m_currentThrow != nullptr)
	{
		std::cout << "Not currently null" << "\n";
	}

	for (auto& e : m_entityManager.getEntities())  //Basically update the previous position of all entities
	{
		e->get<CState>().previousState = e->get<CState>().state;
		if (e->get<CBoundingBox>().iFrames > 0) { e->get<CBoundingBox>().iFrames -= 1; }
		e->get<CTransform>().prevPos = e->get<CTransform>().pos;
	}

	//Main game systems
	m_game.sUserInput();
	if (m_sceneControl.doGUI)       { sGUI(); }
	if (m_sceneControl.doMovement)	{ sMovement(); }
	if (m_sceneControl.doAnimation) { sAnimation(); }
	sLifespan();
	sRayCast();
	sRender(); //Leaving this out of it because I want to have it change to like a different type of a scene situation, ill work on it later
	if (m_sceneControl.doCollision) { sCollision(); }
}

void Scene_Play::sMovement()
{

	//Add velocity to both of the bullet types when theyre moving
	for (auto& e : m_entityManager.getEntities("BulletVerti"))
	{
		auto& velocity = e->get<CTransform>().velocity.x;
		if (velocity < 0) { velocity -= 0.2; }
		else { velocity += 0.2; }
	}
	for (auto& e : m_entityManager.getEntities("BulletHori"))
	{
		auto& velocity = e->get<CTransform>().velocity.y;
		if (velocity < 0) { velocity -= 0.2; }
	}

	//Handle all the raycasting entities
	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->has<CRayCast>()) { continue; }
		auto& rayCast = e->get<CRayCast>();
		auto& transform = e->get<CTransform>();

		if (e->tag() == "spinShroom" && e->has<CBoundingBox>() && e->get<CState>().state != States::longDead)
		{
			if (rayCast.direction == "Left")
			{
				transform.velocity.x = -rayCast.pointCount * 1.5;
			}
			else if (rayCast.direction == "Right")
			{
				transform.velocity.x = rayCast.pointCount * 1.5;
			}
			else { transform.velocity.x = 0; }
			
			//Decrement or increment the turnAround count, and change scale in accordance
			//Made so the enemy can turn and look in that direction
			if (rayCast.turnAroundCount > 0)
			{
				rayCast.turnAroundCount -= 1;
			}
			else if (rayCast.turnAroundCount <= 0 && rayCast.canTurnAround)
			{
				rayCast.turnAroundCount = 180;
				e->get<CTransform>().scale.x *= -1; //Turn the enemy to face the other direction
			}
		}
	}

	//regular left and right movement handling
	auto& velocityY   = player()->get<CTransform>().velocity.y;
	ArialStates& ariealState = player()->get<CState>().aerialState;
	States& playerState = player()->get<CState>().state;
	if(player()->get<CInput>().right)
	{
		if (player()->get<CTransform>().scale.x > 0) { player()->get<CTransform>().scale.x *= -1.0f; } //This changes out the way the sprite is facing based on direction
		if (playerState != States::hurt) { player()->get<CTransform>().velocity.x = m_playerConfig.SPEED; }							   //Change the speed so the sprite moves to the left
		if (ariealState != ArialStates::jump
			&& playerState != States::attack
			&& playerState != States::shoot
			&& playerState != States::doubleJump
			&& playerState != States::hurt
			&& playerState != States::holding
			&& playerState != States::pushing)
		{ player()->get<CState>().state = States::run; }
	}
	if(player()->get<CInput>().left)
	{ 
		if (player()->get<CTransform>().scale.x < 0) { player()->get<CTransform>().scale.x *= -1.0f; } //This changes out the way the sprite is facing based on direction
		if (playerState != States::hurt) { player()->get<CTransform>().velocity.x = -m_playerConfig.SPEED; }
		if (ariealState != ArialStates::jump
			&& playerState != States::attack
			&& playerState != States::shoot
			&& playerState != States::doubleJump
			&& playerState != States::hurt
			&& playerState != States::holding
			&& playerState != States::pushing)
		{ player()->get<CState>().state = States::run;}
	}
	if(player()->get<CInput>().left == false && player()->get<CInput>().right == false)
	{ 
		player()->get<CTransform>().velocity.x = 0; //pause the movement
		//Do a check to see if we can go back to the default state of standing or not
		if (ariealState != ArialStates::jump
			&& playerState != States::hurt
			&& playerState != States::attack
			&& playerState != States::shoot
			&& playerState != States::doubleJump
			&& playerState != States::hurt
			&& playerState != States::holding
			&& playerState != States::holdingBox
			&& playerState != States::pushing)
		{ player()->get<CState>().state = States::stand; }
	}
	
	bool notHoldingABlock = true;

	//Handle all the movement when a block is thrown
	if (m_currentThrow != nullptr)
	{
		CTransform& trans = m_currentThrow->get<CTransform>();
		CTransform& pTrans = player()->get<CTransform>();
		//Resert the hold and push variables in the game engine
		m_currentPush = nullptr;
		m_currentHold = nullptr;

		//Increase upward y velocity as long as it doesnt go above a certain threshold
		if (-trans.velocity.y < m_playerConfig.MAXSPEED and m_currentThrow->get<CState>().state != States::NONgrounded ) //Basically a check simulate upward acceleration
		{

			trans.velocity.y -= m_playerConfig.JUMP / 2;
		}
		//If y velocity is exceeded, then add gravity:
		else
		{
			m_currentThrow->get<CState>().state = States::NONgrounded;
		}

		//Depending on the way the player is facing, change the horizontal velocity
		if (player()->get<CTransform>().scale.x < 0 && trans.velocity.x == 0 && m_currentThrow->get<CState>().state != States::grounded && (trans.velocity.y < 0))
		{
			trans.velocity.x = m_playerConfig.MAXSPEED/ 1.2;
		}
		else if (player()->get<CTransform>().scale.x > 0 && trans.velocity.x == 0 && m_currentThrow->get<CState>().state != States::grounded && (trans.velocity.y < 0))
		{
			trans.velocity.x = -m_playerConfig.MAXSPEED/ 1.2 ;
		}

		//If collide with ground, reset the hold status of the throw entity
		if (m_currentThrow->get<CState>().state == States::grounded)
		{
			m_currentThrow = nullptr;
		}
	}

	//Handle all the movement of a block thats held
	if (m_currentPush != nullptr && m_currentHold != nullptr)
	{
		Vec2f& blockPos = m_currentPush->get<CTransform>().pos;
		Vec2f& playerPos = player()->get<CTransform>().pos;

		//Place the current block above the player's head if they have click the left mouse button
		blockPos.x = playerPos.x;
		blockPos.y = playerPos.y - player()->get<CBoundingBox>().size.x;
		notHoldingABlock = false;
		player()->get<CState>().state = States::holding;
	}

	//Lets handle all the movement of the block
	if (m_currentPush != nullptr && notHoldingABlock)
	{
		Vec2f& blockPos = m_currentPush->get<CTransform>().pos;
		Vec2f& playerPos = player()->get<CTransform>().pos;

		//Check to see if pushing from left or right side
		if (blockPos.x > playerPos.x) //Box is getting pushed from the left
		{
			//The minus is to maintain the collision logic?
			blockPos.x = playerPos.x + player()->get<CBoundingBox>().size.x;
		}
		else if (blockPos.x < playerPos.x) //Box is getting pushed from the Right
		{
			//The minus is to maintain the collision logic?
			blockPos.x = playerPos.x - player()->get<CBoundingBox>().size.x;
		}

		bool sisyphus = StillPushing(player(), m_currentPush);
		if (!sisyphus)
		{
			m_currentPush = nullptr;
			player()->get<CState>().state = States::stand;
			player()->get<CState>().isPushing = false;
		}
	}

	//Handle Gravity for any thing that isnt the player
	for (auto& mAble : m_entityManager.getEntities())
	{
		if (!(checkTypeOfTile(mAble->tag()) == "MoveAble") || mAble == m_currentHold) { continue; }

		// Only apply gravity if the block is NOT grounded
		if (mAble->get<CState>().state == States::NONgrounded)
		{
			mAble->get<CTransform>().velocity.y += m_playerConfig.GRAVITY;
		}
		else if(mAble->get<CState>().state == States::grounded)
		{
			// If grounded, reset vertical velocity to prevent sinking
			mAble->get<CTransform>().velocity.y = 0;
		}
	}


	//Jump handling
	if (player()->get<CInput>().canJump && player()->get<CInput>().jump)
	{
		player()->get<CState>().aerialState = ArialStates::jump;                              //Change state for animation
		if (-velocityY < m_playerConfig.MAXSPEED && player()->get<CInput>().jump && playerState != States::doubleJump)  //Basically a check simulate upward acceleration
		{
			velocityY -= m_playerConfig.JUMP;
		}
		else                                                                       //If you fail the check, it doesnt let you jump again
		{
			player()->get<CInput>().canJump = false;
		}
	}
	else if (ariealState == ArialStates::jump) //Add gravity based on the conditions
	{
		velocityY += player()->get<CGravity>().gravity;
	}
	else if (ariealState == ArialStates::grounded)
	{
		velocityY = 0;
	}

}

void Scene_Play::sLifespan()
{
	//TODO: Check lifespawn of entities that have them, and destroy them if they go over
	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->has<CLifespan>())
		{
			continue;
		}

		e->get<CLifespan>().lifeSpan -= 1;
		if (e->get<CLifespan>().lifeSpan <= 0)
		{
			e->destroy();
		}
	}
}

void Scene_Play::sCollision() //AABB Collision type
{
	//The two of these are to do the collisions for both ink bullet types
	for (auto& bullet : m_entityManager.getEntities("BulletHori"))
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (!e->has<CBoundingBox>() || bullet == e) { continue; }
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(bullet, e);
			Vec2f boxOverLap = Physics::GetOverlap(bullet, e);
			/*std::cout << "Previous Overlap" + std::to_string(previousBoxOverLap.x) << std::endl;
			std::cout << "Overlap" + std::to_string(boxOverLap.x) << std::endl;*/

			//horizontal collisions
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and bullet->get<CTransform>().pos.x >= e->get<CTransform>().pos.x)
			{
				bullet->get<CTransform>().velocity.x = 0;
				bullet->get<CState>().state = States::hit;
			}
			else if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and bullet->get<CTransform>().pos.x < e->get<CTransform>().pos.x)
			{
				bullet->get<CTransform>().velocity.x = 0;
				bullet->get<CState>().state = States::hit;
			}
		}
	}
	for (auto& bullet : m_entityManager.getEntities("BulletVerti"))
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (!e->has<CBoundingBox>() || bullet == e || e == player()) { continue; }
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(bullet, e);
			Vec2f boxOverLap = Physics::GetOverlap(bullet, e);

			//horizontal collisions
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and bullet->get<CTransform>().pos.y > e->get<CTransform>().pos.y)
			{
				bullet->get<CTransform>().velocity.y = 0;
				bullet->get<CState>().state = States::hit;
			}
			else if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and bullet->get<CTransform>().pos.y < e->get<CTransform>().pos.y)
			{
				bullet->get<CTransform>().velocity.y = 0;
				bullet->get<CState>().state = States::hit;
			}
		}
	}

	//The collision statment that determines whether or not the player is dead
	if (player()->get<CTransform>().pos.y + player()->get<CBoundingBox>().halfsize.y > m_game.window().getSize().y - m_windowDifferenceY + 4000)
	{

		player()->get<CState>().state = States::dead;
		player()->get<CTransform>().velocity.x = 0;
		player()->get<CTransform>().velocity.y = 0;
		player()->get<CGravity>().gravity = 0;
		m_sceneControl.doMovement = false;
	}
	
	//This is the collision system for the player
	for (auto& p : m_entityManager.getEntities("Player"))
	{
		bool isOnGround = false;
		auto& pTransform = p->get<CTransform>();
		auto& pVelocity  = pTransform.velocity;
		auto& pPos       = pTransform.pos;
		auto& pState     = p->get<CState>();
		auto& pAnim      = p->get<CAnimation>().animation;
		//This if Statement makes it so you cant go past x=0
		if (pPos.x - pAnim.getSize().x / 2 < 0)
		{
			p->get<CTransform>().pos.x = 0 + p->get<CAnimation>().animation.getSize().x / 2;
		}

		for (auto& t : m_entityManager.getEntities()) //Check all of the entities collisions
		{

			if (!t->has<CBoundingBox>() || p == t || t->tag() == "BulletVerti" || t->tag() == "BulletHori" || t->tag() == "attackBox" || t->tag()=="ShipIdle" || t->tag() == "DustCloud") { continue; } //Basically just dont collide with bullet, or yourself or decoration

			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(p, t);                                                 //Sees if the previous bounding boxes overlap
			Vec2f boxOverLap = Physics::GetOverlap(p, t);                                                                 //Sees if the current bounding boxes collided
			Vec2f boxOverLapShifted = Physics::GetShiftedOverlap(p, t);

			// Instead of completely skipping, handle it differently
			if (m_currentPush != nullptr && m_currentPush == t)
			{
				// Allow vertical collision resolution but handle horizontal differently
				// This ensures the player stays on top of the block
				if (previousBoxOverLap.y <= 0 && boxOverLap.y > 0 && pPos.y < t->get<CTransform>().pos.y)
				{
					player()->get<CInput>().canJump = true;
					player()->get<CInput>().canShoot = true;
					pState.aerialState = ArialStates::grounded;
					pVelocity.y = 0;
					pPos.y -= boxOverLap.y;
				}
				continue; // Still skip horizontal collision to allow pushing
			}
			
			//Ground collision check
			if (boxOverLapShifted.y > 0 and pPos.y < t->get<CTransform>().pos.y && pVelocity.y >= 0) //Checks collision with the bottom side of a block
			{
				isOnGround = true;
				//Player can do all of these things if something is considered the ground and it collides with the top of it
				player()->get<CInput>().canJump = true;
				player()->get<CInput>().canShoot = true;
				pState.aerialState = ArialStates::grounded;
			}
			if (!isOnGround)
			{
				pState.aerialState = ArialStates::jump;
			}
			
			//Vertical collisions
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pPos.y > t->get<CTransform>().pos.y) //Checks collision with the top of a block
			{

				if (t->get<CState>().isEnemy) { pVelocity.y = 10; pVelocity.x = 10; pState.state = States::hurt; }
				pVelocity.y = 0.0f;
				pPos.y += boxOverLap.y;

			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pPos.y < t->get<CTransform>().pos.y) //Checks collision with the bottom side of a block
			{

				pVelocity.y = 0;
				pPos.y -= boxOverLap.y;
				if (t->get<CState>().isEnemy) { pVelocity.y = -10; pVelocity.x = -10; pState.state = States::hurt; }

			}
			else if (pState.aerialState == ArialStates::grounded && pVelocity.y > m_playerConfig.GRAVITY) //This caviate is basically sort of coyote jumping, allowing finite jump delay
			{

				player()->get<CInput>().canJump = false;

			}
			
			//horizontal collisions
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pPos.x > t->get<CTransform>().pos.x)               //Collision with the right side of a block (doesnt work fully)
			{

				//OH MY FUCKING GOD THIS STUPID ASS FUCK SHIT ASS CODE IS GONNA MAKE ME BLOW MY BRAINS OUT
				if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pPos.y < t->get<CTransform>().pos.y) { pPos.y -= boxOverLap.y;}
				if (t->get<CState>().isEnemy) { pVelocity.x = +10; pState.state = States::hurt; }
				pPos.x += boxOverLap.x;
				
				
				//Check for collision with type of Object
				const std::string check = checkTypeOfTile(t->tag());
				if (check == "MoveAble" && player()->get<CState>().aerialState == ArialStates::grounded && m_currentHold == nullptr)
				{
					//Make the item you collide with be the held item
					m_currentPush = t;
					p->get<CState>().isPushing = true;
					p->get<CState>().state = States::pushing;
				}
			}
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pPos.x < t->get<CTransform>().pos.x) //Checks for collision with the left side of a block
			{
				if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pPos.y < t->get<CTransform>().pos.y) { pPos.y -= boxOverLap.y; }
				pPos.x -= boxOverLap.x;
				if (t->get<CState>().isEnemy) { pVelocity.x = -10; pVelocity.y -= 5; pState.state = States::hurt; }
				
				
				//Check for collision with type of Object
				const std::string check = checkTypeOfTile(t->tag());
				if (check == "MoveAble" && player()->get<CState>().aerialState == ArialStates::grounded && m_currentHold == nullptr)
				{
					//Make the item you collide with be the held item
					m_currentPush = t;
					p->get<CState>().isPushing = true;
					p->get<CState>().state = States::pushing;
				}

			}
		}
	}

	//Collision System for any entity named "TallMushroom"
	for (auto& e : m_entityManager.getEntities("TallMushroom"))
	{
		if (e->get<CBoundingBox>().iFrames > 0) { continue; }
		bool beenHit = false;
		for (auto& bulletV    : m_entityManager.getEntities("BulletVerti"))
		{
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(e, bulletV);
			Vec2f boxOverLap = Physics::GetOverlap(e, bulletV);

			//horizontal collisions
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and e->get<CTransform>().pos.y > bulletV->get<CTransform>().pos.y)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and e->get<CTransform>().pos.y < bulletV->get<CTransform>().pos.y)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}
		for (auto& bulletH    : m_entityManager.getEntities("BulletHori"))
		{
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(e, bulletH);
			Vec2f boxOverLap = Physics::GetOverlap(e, bulletH);
			/*std::cout << "Previous Overlap" + std::to_string(previousBoxOverLap.x) << std::endl;
			std::cout << "Overlap" + std::to_string(boxOverLap.x) << std::endl;*/

			//horizontal collisions
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and e->get<CTransform>().pos.x >= bulletH->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and e->get<CTransform>().pos.x < bulletH->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}
		for (auto& attackBox  : m_entityManager.getEntities("attackBox"))
		{
			Vec2f boxOverLap = Physics::GetOverlap(e, attackBox);
			
			//horizontal collisions
			if (boxOverLap.x > 0 and e->get<CTransform>().pos.x >= attackBox->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (boxOverLap.x > 0 and e->get<CTransform>().pos.x < attackBox->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}

		if (beenHit && e->get<CBoundingBox>().iFrames <= 0)
		{ 
			e->get<CHealth>().currentHealth -= 15; 
			e->get<CBoundingBox>().iFrames = 20;
		}
		if (e->get<CHealth>().currentHealth < 0) { e->get<CState>().state = States::dead; }
	}
	for (auto& e : m_entityManager.getEntities("spinShroom"))
	{
		if (e->get<CBoundingBox>().iFrames > 0) { continue; }
		bool beenHit = false;
		for (auto& bulletV : m_entityManager.getEntities("BulletVerti"))
		{
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(e, bulletV);
			Vec2f boxOverLap = Physics::GetOverlap(e, bulletV);

			//horizontal collisions
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and e->get<CTransform>().pos.y > bulletV->get<CTransform>().pos.y)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and e->get<CTransform>().pos.y < bulletV->get<CTransform>().pos.y)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}
		for (auto& bulletH : m_entityManager.getEntities("BulletHori"))
		{
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(e, bulletH);
			Vec2f boxOverLap = Physics::GetOverlap(e, bulletH);
			/*std::cout << "Previous Overlap" + std::to_string(previousBoxOverLap.x) << std::endl;
			std::cout << "Overlap" + std::to_string(boxOverLap.x) << std::endl;*/

			//horizontal collisions
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and e->get<CTransform>().pos.x >= bulletH->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and e->get<CTransform>().pos.x < bulletH->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}
		for (auto& attackBox : m_entityManager.getEntities("attackBox"))
		{
			Vec2f boxOverLap = Physics::GetOverlap(e, attackBox);

			//horizontal collisions
			if (boxOverLap.x > 0 and e->get<CTransform>().pos.x >= attackBox->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;

			}
			else if (boxOverLap.x > 0 and e->get<CTransform>().pos.x < attackBox->get<CTransform>().pos.x)
			{
				beenHit = true;
				e->get<CState>().state = States::hurt;
			}
		}

		if (beenHit && e->get<CBoundingBox>().iFrames <= 0)
		{
			e->get<CHealth>().currentHealth -= 15;
			e->get<CBoundingBox>().iFrames = 20;
		}
		if (e->get<CHealth>().currentHealth < 0) { e->get<CState>().state = States::dead; }
	}

	//gravity Check for every block with gravity
	for (auto& gravityEntity : m_entityManager.getEntities())
	{
		bool isOnGround = false;
		if ((!(checkTypeOfTile(gravityEntity->tag()) == "MoveAble")) || gravityEntity == m_currentPush || gravityEntity == m_currentThrow) { continue; }
		if (!gravityEntity->has<CState>()) { gravityEntity->add<CState>(); }
		auto& gravityTransform = gravityEntity->get<CTransform>();
		
		for (auto& nonGravity : m_entityManager.getEntities())
		{
			if (nonGravity == gravityEntity || nonGravity->tag() == "DustCloud" || nonGravity->tag() == "playerHeart") { continue; }

			auto& nonGravityTransform = nonGravity->get<CTransform>();
			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(gravityEntity, nonGravity);                                                 //Sees if the previous bounding boxes overlap
			Vec2f boxOverLap = Physics::GetOverlap(gravityEntity, nonGravity);
			Vec2f boxOverLapShifted = Physics::GetShiftedOverlap(gravityEntity, nonGravity);
			
			//Vertical collisions

			if (boxOverLapShifted.y > 0 and gravityTransform.pos.y < nonGravityTransform.pos.y) //Checks collision with the bottom side of a block
			{
				gravityEntity->get<CState>().state = States::grounded;
				isOnGround = true;
			}
			if(!isOnGround)
			{
				gravityEntity->get<CState>().state = States::NONgrounded;
			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and gravityTransform.pos.y < nonGravityTransform.pos.y && nonGravityTransform.velocity.y == 0) //Checks for collision with the left side of a block
			{
				gravityTransform.velocity.y = 0;
				gravityTransform.pos.y -= boxOverLap.y;
				spawnDustCloud(gravityEntity);
			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and gravityTransform.pos.y > nonGravityTransform.pos.y && nonGravityTransform.velocity.y == 0)//Collision with the right side of a block (doesnt work fully)
			{
				gravityTransform.velocity.y = 0;
				gravityTransform.pos.y += boxOverLap.y;

			}


		}
	}
	
	//This one checks to see collision between moveable and other things
	if (m_currentPush != nullptr)
	{
		auto& pushPos = m_currentPush->get<CTransform>().pos;
		auto& pTransform = player()->get<CTransform>().pos;
		bool isOnGround = false;
		bool collisionDetected = false;

		for (auto& entity : m_entityManager.getEntities())
		{
			if (entity == player() or entity->tag() == "attackBox") { continue; }
			auto& entityPos = entity->get<CTransform>().pos;

			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(m_currentPush, entity); //Sees if the previous bounding boxes overlap
			Vec2f boxOverLap = Physics::GetOverlap(m_currentPush, entity);
			Vec2f boxOverLapShifted = Physics::GetShiftedOverlap(m_currentPush, entity);

			if (boxOverLapShifted.y > 0 and pushPos.y < entityPos.y) //Checks collision with the bottom side of a block
			{
				m_currentPush->get<CState>().state = States::grounded;
				isOnGround = true;
			}
			if (!isOnGround)
			{
				m_currentPush->get<CState>().state = States::NONgrounded;
			}
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pushPos.x < entityPos.x) //Checks for collision with the left side of a block
			{
				pushPos.x -= boxOverLap.x;
				m_currentPush->get<CTransform>().velocity.y = 0;
				pTransform.x -= boxOverLap.x;
				collisionDetected = true;

			}
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pushPos.x > entityPos.x)//Collision with the right side of a block (doesnt work fully)
			{
				pushPos.x += boxOverLap.x;
				m_currentPush->get<CTransform>().velocity.y = 0;
				pTransform.x += boxOverLap.x;
				collisionDetected = true;
			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pushPos.y < entityPos.y) //Checks for collision with the left side of a block
			{
				pushPos.y -= boxOverLap.y;
				m_currentPush->get<CTransform>().velocity.y = 0;
				pTransform.y -= boxOverLap.y;
				collisionDetected = true;

			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pushPos.y > entityPos.y)//Collision with the right side of a block (doesnt work fully)
			{
				pushPos.y += boxOverLap.y;
				m_currentPush->get<CTransform>().velocity.y = 0;
				pTransform.y += boxOverLap.y;
				collisionDetected = true;
			}
			if (collisionDetected)
			{
				m_currentHold = nullptr;
				player()->get<CTransform>().velocity.x = 0;
				player()->get<CTransform>().velocity.y = 0;
			}
		}
	}
	//Check collisions between current thrown object:
	if (m_currentThrow != nullptr)
	{
		auto& pushPos = m_currentThrow->get<CTransform>().pos;
		auto& pTransform = player()->get<CTransform>().pos;
		bool isOnGround = false;
		bool collisionDetected = false;

		for (auto& entity : m_entityManager.getEntities())
		{
			if (entity->tag() == "attackBox") { continue; }
			auto& entityPos = entity->get<CTransform>().pos;

			Vec2f previousBoxOverLap = Physics::GetPreviousOverlap(m_currentThrow, entity); //Sees if the previous bounding boxes overlap
			Vec2f boxOverLap = Physics::GetOverlap(m_currentThrow, entity);
			Vec2f boxOverLapShifted = Physics::GetShiftedOverlap(m_currentThrow, entity);

			if (boxOverLapShifted.y > 0 and pushPos.y < entityPos.y && m_currentThrow->get<CTransform>().velocity.y >= 0) //Checks collision with the bottom side of a block
			{
				m_currentThrow->get<CState>().state = States::grounded;
				isOnGround = true;
			}
			if (!isOnGround && m_currentThrow->get<CTransform>().velocity.y >= 0)
			{
				m_currentThrow->get<CState>().state = States::NONgrounded;
			}
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pushPos.x < entityPos.x) //Checks for collision with the left side of a block
			{
				pushPos.x -= boxOverLap.x;
				m_currentThrow->get<CTransform>().velocity.y = 0;
				m_currentThrow->get<CTransform>().velocity.x = 0;
				//pTransform.x -= boxOverLap.x;
				collisionDetected = true;
				spawnDustCloud(m_currentThrow);

			}
			if (previousBoxOverLap.x <= 0 and boxOverLap.x > 0 and pushPos.x > entityPos.x)//Collision with the right side of a block (doesnt work fully)
			{
				pushPos.x += boxOverLap.x;
				m_currentThrow->get<CTransform>().velocity.y = 0;
				m_currentThrow->get<CTransform>().velocity.x = 0;
				//pTransform.x += boxOverLap.x;
				collisionDetected = true;
				spawnDustCloud(m_currentThrow);
			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pushPos.y < entityPos.y) //Checks for collision with the left side of a block
			{

				pushPos.y -= boxOverLap.y;
				m_currentThrow->get<CTransform>().velocity.y = 0;
				m_currentThrow->get<CTransform>().velocity.x = 0;
				//pTransform.y -= boxOverLap.y;
				collisionDetected = true;
				spawnDustCloud(m_currentThrow);

			}
			if (previousBoxOverLap.y <= 0 and boxOverLap.y > 0 and pushPos.y > entityPos.y)//Collision with the right side of a block (doesnt work fully)
			{
				pushPos.y += boxOverLap.y;
				m_currentThrow->get<CTransform>().velocity.y = 0;
				m_currentThrow->get<CTransform>().velocity.x = 0;
				//pTransform.y += boxOverLap.y;
				collisionDetected = true;
				spawnDustCloud(m_currentThrow);
			}
			if (m_currentThrow->get<CTransform>().pos.y + player()->get<CBoundingBox>().halfsize.y > m_game.window().getSize().y - m_windowDifferenceY + 4000)
			{
				m_currentThrow->get<CState>().state = States::grounded;
				collisionDetected = true;
			}
		}
	}
}

void Scene_Play::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PAUSE")                 
		{ 
			m_sceneControl.doCollision = !m_sceneControl.doCollision;
			m_sceneControl.doGUI       = !m_sceneControl.doGUI;
			m_sceneControl.doAnimation = !m_sceneControl.doAnimation;
			m_sceneControl.doLifeSpan  = !m_sceneControl.doLifeSpan;
			m_sceneControl.doMovement  = !m_sceneControl.doMovement;
			player()->get<CTransform>().velocity = Vec2f(0, 0);
		}
		else if (action.name() == "TOGGLE_TEXTURE")							   { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")						   { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")						       { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "TOGGLE_RAYCAST")							   { m_drawRaycast = !m_drawRaycast; }
		else if (action.name() == "PAUSE")									   { m_paused = !m_paused; }
		else if (action.name() == "ESCAPE")									   {m_game.changeScene("MENU", std::make_shared<Scene_Menu>(m_game), false);}
		else if (action.name() == "QUIT")									   { m_hasEnded = true; }
		else if (action.name() == "LEFT")									   {player()->get<CInput>().left = true;}
		else if (action.name() == "RIGHT")									   {player()->get<CInput>().right = true;}
		else if (action.name() == "JUMP")									   { if (player()->get<CInput>().canJump) { player()->get<CInput>().jump = true; }spawnDustCloud(player());}
		else if (action.name() == "RUN")			                           { m_playerConfig.SPEED += 2; }
		else if (action.name() == "FPS")                                       { m_showFPS = !m_showFPS; }
		else if (action.name() == "ATTACK" && m_currentHold != nullptr)        { m_currentThrow = m_currentPush; m_currentThrow->get<CState>().state = States::idle; player()->get<CState>().state = States::stand;}
		else if (action.name() == "ATTACK" && m_currentPush != nullptr)		   { m_currentHold = m_currentPush; }
		else if (action.name() == "ATTACK")									   { player()->get<CState>().state = States::attack;}
		else if (action.name() == "SHOOT" && m_currentHold != nullptr)         { putDownBlock(); }
		else if (action.name() == "SHOOT"&&
		player()->get<CState>().aerialState== ArialStates::jump
		&& player()->get<CInput>().canShoot
		&& player()->get<CState>().state!= States::hurt)    { player()->get<CState>().state = States::doubleJump; player()->get<CInput>().canShoot = false; }
		
		else if (action.name() == "SHOOT"
		&& player()->get<CInput>().canShoot
		&& player()->get<CState>().state != States::hurt)  { player()->get<CState>().state = States::shoot; }
	}
	else if (action.type() == "END")
	{
		if      (action.name() == "LEFT")             {player()->get<CInput>().left     = false;}
		else if (action.name() == "RIGHT")            {player()->get<CInput>().right    = false;}
		else if (action.name() == "JUMP")             {player()->get<CInput>().jump = false; player()->get<CInput>().canJump = false;}
		else if (action.name() == "RUN")			  { m_playerConfig.SPEED -= 2; }
	}
}

void Scene_Play::sAnimation()
{
	//This updates every single one of the animations, moving it to the next frame or ending it
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->has<CAnimation>() && e->get<CAnimation>().repeat)
		{
			e->get<CAnimation>().animation.update();
		}
		else if (!e->get<CAnimation>().animation.hasEnded())
		{
			e->get<CAnimation>().animation.update();
		}
	}

	//Refrences to a bunch of different playerAsspects
	auto& currentAnimation    = player()->get<CAnimation>().animation.getName();
	States& playerState         = player()->get<CState>().state;
	States& previousPlayerState = player()->get<CState>().previousState;
	CTransform& playerTransform     = player()->get<CTransform>();

	
	//The two for-loops to handle player logic/animation
	for (auto& player : m_entityManager.getEntities("Player"))
	{

		if (player->get<CState>().aerialState == ArialStates::grounded and playerState != States::dead)
		{

			if (
				playerState == States::holdingBox
				&& player->get<CTransform>().velocity.x == 0
				&& currentAnimation != "SquidHoldBox"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidHoldBox"), true);
			}
			//Do the animation for idlePushing if youre pushing, but there is horizontal movement
			if (
				(playerState == States::pushing)
				&& player->get<CTransform>().velocity.x != 0
				&& currentAnimation != "SquidPushWalk"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidPushWalk"), true);
			}
			//Do the animation for idlePushing if youre pushing, but no horizontal movement
			else if (
				(playerState == States::pushing)
				&& player->get<CTransform>().velocity.x == 0
				&& currentAnimation != "SquidPushIdle"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidPushIdle"), true);
			}

			//Do the animation for the squid holding the block over its head if these conditions are met
			if  (
				playerState == States::holding && player->get<CTransform>().velocity.x != 0
				&& currentAnimation != "SquidHoldWalk"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidHoldWalk"), true);
			}
			else if (
				playerState == States::holding && player->get<CTransform>().velocity.x == 0
				&& currentAnimation != "SquidHoldIdle"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidHoldIdle"), true);
			}
			
			//Do the base run /  walk animations by checling the player state and current animation
			if (
				playerState == States::run
				&& currentAnimation != "Run"
				&& playerState != States::shoot
				&& playerState != States::attack)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Run"), true);
			}

			else if (
				playerState == States::stand
				&& currentAnimation != "Stand"
				&& playerState != States::shoot
				&& playerState != States::attack)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Stand"), true);
			}

			if (
				playerState == States::attack
				&& playerState != States::shoot
				&& currentAnimation != "Hit")
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Hit"), false);
				spawnAttack(player);
			}

			else if (player->get<CAnimation>().animation.hasEnded() && playerState == States::attack)
			{
				player->get<CState>().state = States::stand;
				for (auto& e : m_entityManager.getEntities("attackBox")) { e->destroy(); }
			}

			if (
				playerState == States::shoot
				&& currentAnimation != "Shoot"
				&& currentAnimation != "Hit"
				&& player->get<CState>().aerialState != ArialStates::jump)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Shoot"), false);
			}
			else if (player->get<CAnimation>().animation.hasEnded() && playerState == States::shoot)
			{
				player->get<CState>().state = States::stand;
				std::string direction = "Hori";
				spawnBullet(player, direction);
			}

		}
		else if (player->get<CState>().aerialState == ArialStates::jump and playerState != States::dead and playerState != States::hurt)
		{
			if (
				playerState == States::holdingBox
				&& player->get<CTransform>().velocity.x == 0
				&& currentAnimation != "SquidHoldBox"
				)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidHoldBox"), true);
			}
			if (currentAnimation != "SquidHoldJump" && m_currentHold != nullptr)
			{
				player->add<CAnimation>(m_game.assets().getAnimation("SquidHoldJump"), true);
			}
			if (currentAnimation != "Jump" && playerState != States::doubleJump && playerState != States::jumpSpin && currentAnimation != "SquidHoldJump")
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Jump"), true);
			}
			if (playerState == States::doubleJump && currentAnimation != "JumpShoot" && playerState != States::shoot && playerState != States::attack)
			{
				player->get<CGravity>().gravity = 0;
				playerTransform.velocity.y = -2;
				player->add<CAnimation>(m_game.assets().getAnimation("JumpShoot"), false);
			}
			else if (player->get<CAnimation>().animation.hasEnded() && playerState == States::doubleJump)
			{
				std::string direction = "Verti";
				spawnBullet(player, direction);
				player->get<CGravity>().gravity = m_playerConfig.GRAVITY;
				player->get<CTransform>().velocity.y -= 20;
				player->add<CAnimation>(m_game.assets().getAnimation("JumpSpin"), true);
				playerState = States::jumpSpin;
			}
			if ((playerState == States::attack && currentAnimation == "Jump") || (playerState == States::grounded && currentAnimation == "Jump"))
			{
				player->get<CGravity>().gravity = m_playerConfig.GRAVITY;
			}
		}

		if (playerState == States::hurt && currentAnimation != "Hurt")
		{
			player->add<CAnimation>(m_game.assets().getAnimation("Hurt"), false);
			player->get<CGravity>().gravity = m_playerConfig.GRAVITY;
		}
		else if (player->get<CAnimation>().animation.hasEnded() && currentAnimation == "Hurt")
		{
			playerState = States::stand;
			playerTransform.velocity = Vec2f(0, 0);
		}

		if (playerState == States::dead)
		{
			if (currentAnimation != "Died")
			{
				player->add<CAnimation>(m_game.assets().getAnimation("Died"), false);
			}
			else if (player->get<CAnimation>().animation.hasEnded())
			{
				player->get<CTransform>().pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, player);
				player->get<CGravity>().gravity = m_playerConfig.GRAVITY;
				player->get<CState>().state = States::stand;
				m_sceneControl.doMovement = true;


				if (m_entityManager.getEntities("playerHeart").empty())
				{
					for (int i = 0; i < m_playerConfig.HEARTS; i++)
					{
						auto uiElement = m_entityManager.addEntity("playerHeart");
						uiElement->add<CUserInterface>(true);
						uiElement->add<CState>(States::notDying);
						uiElement->add<CAnimation>(m_game.assets().getAnimation("tentaIdle"), true);
						uiElement->add<CTransform>(gridToMidPixel(i, 0, uiElement), Vec2f(0, 0), Vec2f(1, 1), 0);
					}
				}
				else
				{
					for (int i = 0; i < m_playerConfig.HEARTS - m_entityManager.getEntities("playerHeart").size(); i++)
					{
						auto uiElement = m_entityManager.addEntity("playerHeart");
						uiElement->add<CUserInterface>(true);
						uiElement->add<CState>("notDying");
						uiElement->add<CAnimation>(m_game.assets().getAnimation("tentaIdle"), true);
						uiElement->add<CTransform>(gridToMidPixel(i, 0, uiElement), Vec2f(0, 0), Vec2f(1, 1), 0);
					}
				}
			}
		}

		if (playerState != States::hurt && previousPlayerState == States::hurt)
		{
			m_entityManager.getEntities("playerHeart").back()->get<CState>().state = States::dying;
		}
	}
	
	//The tentacles
	for (auto& e : m_entityManager.getEntities("playerHeart"))
	{
		States& eState = e->get<CState>().state;
		auto& eAnim = e->get<CAnimation>().animation;
		if (eState == States::dying && eAnim.getName() != "tentaDied")
		{
			e->add<CAnimation>(m_game.assets().getAnimation("tentaDied"), false);
		}
		else if (eState == States::dying && eAnim.hasEnded())
		{
			if (m_entityManager.getEntities("playerHeart").size() == 1)
			{
				eState = States::died;
				playerState = States::dead;
				player()->get<CTransform>().velocity.x = 0;
				player()->get<CTransform>().velocity.y = 0;
				player()->get<CGravity>().gravity = 0;
				m_sceneControl.doMovement = false;
				e->destroy();
			}
			else
			{
				e->destroy();
			}
		}
	}
	for (auto& e : m_entityManager.getEntities("DustCloud"))
	{
		if (e->get<CAnimation>().animation.hasEnded()) {e->destroy();}
	}
	for (auto& e : m_entityManager.getEntities("ShipIdle"))
	{
		auto& state = e->get<CState>();
		auto&  anim = e->get<CAnimation>();
		if (state.state == States::abduct && anim.animation.getName() != "ShipBeamDown")
		{
			e->add<CAnimation>(m_game.assets().getAnimation("ShipBeamDown"), false);
		}
	}
	
	//The bullet logic/action/animation
	for (auto& bullet : m_entityManager.getEntities("BulletHori"))
	{
		auto& currentAnimation = bullet->get<CAnimation>().animation.getName();
		if (bullet->get<CState>().state == States::hit && currentAnimation != "InkBallHitH")
		{
			bullet->get<CTransform>().velocity.x = 0;
			bullet->add<CAnimation>(m_game.assets().getAnimation("InkBallHitH"), false);
		}
		else if (bullet->get<CAnimation>().animation.hasEnded() && currentAnimation == "InkBallHitH")
		{
			bullet->destroy();
		}
	}
	for (auto& bullet : m_entityManager.getEntities("BulletVerti"))
	{
		auto& currentAnimation = bullet->get<CAnimation>().animation.getName();
		if (bullet->get<CState>().state == States::hit && currentAnimation != "InkBallHitV")
		{
			bullet->get<CTransform>().velocity.x = 0;
			bullet->add<CAnimation>(m_game.assets().getAnimation("InkBallHitV"), false);
		}
		else if (bullet->get<CAnimation>().animation.hasEnded() && currentAnimation == "InkBallHitV")
		{
			bullet->destroy();
		}
	}
	
	//Animation/logic/action for enemies
	for (auto& enemy : m_entityManager.getEntities("TallMushroom"))
	{
		auto& currentAnim = enemy->get<CAnimation>().animation.getName();
		States& enemyState  = enemy->get<CState>().state;
		if (enemyState == States::dead && currentAnim != "TallDeath")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("TallDeath"), false);
			enemyState = States::longDead;
			enemy->remove<CBoundingBox>();
		}
		else if (enemyState == States::longDead && enemy->get<CAnimation>().animation.hasEnded())
		{
			continue;
		}

		if (enemy->get<CState>().state == States::hurt && currentAnim != "TallHurt")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("TallHurt"), false);
		}
		else if (enemy->get<CAnimation>().animation.hasEnded() && currentAnim == "TallHurt")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("TallIdleSleep"), true);
			enemy->get<CState>().state = States::idle;
		}
	}
	for (auto& enemy : m_entityManager.getEntities("spinShroom"))
	{
		auto& currentAnim = enemy->get<CAnimation>().animation.getName();
		auto& enemyState = enemy->get<CState>().state;

		if (enemyState == States::walk && currentAnim != "spinWalk" && currentAnim != "spinDeath")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("spinWalk"), true);
		}
		else if (enemy->get<CAnimation>().animation.hasEnded() && currentAnim == "spinWalk" && currentAnim != "spinDeath" && currentAnim != "spinHurt")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("spinIdle"), true);
			enemyState = States::idle;
		}

		if (enemyState == States::dead && currentAnim != "spinDeath")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("spinDeath"), false);
			enemyState = States::longDead;
			enemy->remove<CBoundingBox>();
		}
		else if (enemyState == States::longDead && enemy->get<CAnimation>().animation.hasEnded())
		{
			enemy->get<CTransform>().velocity.x = 0;

			continue;
		}

		if (enemyState == States::hurt && currentAnim != "spinHurt")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("spinHurt"), false);
		}
		else if (enemy->get<CAnimation>().animation.hasEnded() && currentAnim == "spinHurt")
		{
			enemy->add<CAnimation>(m_game.assets().getAnimation("spinIdle"), true);
			enemyState = States::idle;
		}

	}
}

void Scene_Play::sRayCast()
{
	for (auto& primary : m_entityManager.getEntities())
	{
		//Check to see if the primary Entity has a rayCast
		if (!primary->has<CRayCast>()) { continue; }

		//Get a refrence ti primary raycast
		auto& pRayCast = primary->get<CRayCast>();

		//std::vector<Vec2f> polygonPoints;
		std::vector<std::pair<Vec2f, Vec2f>> worldPolygonLines;

		//This essentially gets and stores all of the polygon points for each entity
		for (int i = 0; i < pRayCast.collisionShape.getPointCount(); i++)
		{
			Vec2f p1(pRayCast.collisionShape.getPoint(i).x + pRayCast.collisionShape.getPosition().x, pRayCast.collisionShape.getPoint(i).y + pRayCast.collisionShape.getPosition().y);
			Vec2f p2(pRayCast.collisionShape.getPoint((i + 1) % pRayCast.collisionShape.getPointCount()).x + pRayCast.collisionShape.getPosition().x, pRayCast.collisionShape.getPoint((i + 1) % pRayCast.collisionShape.getPointCount()).y + pRayCast.collisionShape.getPosition().y);
			worldPolygonLines.push_back({ p1, p2 });
		}
		
		//Draw out any of the rays dependent on the character
		for (auto& secondary : m_entityManager.getEntities())
		{
			//Check to see if the primary Entity has a rayCast or is the same as the secondary Entity
			if (!secondary->has<CRayCast>() || secondary == primary) { continue; }
			
			//Get a refrence to secondary raycast
			auto& sRayCast = secondary->get<CRayCast>();

			//Cast out a ray specific to the spinShroom behavior logic in the game model
			if (secondary->tag() == "spinShroom")
			{
				//Add a single endpoint to the cast rays array
				std::vector<Vec2f> addPoints;

				//Change the end of the raycast ray based on where the mushroom is looking based on its scale
				if (secondary->get<CTransform>().scale.x > 0)
				{
					addPoints.push_back(Vec2f(secondary->get<CTransform>().pos.x - m_game.window().getSize().x / 3, secondary->get<CTransform>().pos.y + sRayCast.yOffset));
				}
				else if (secondary->get<CTransform>().scale.x < 0)
				{
					addPoints.push_back(Vec2f(secondary->get<CTransform>().pos.x + m_game.window().getSize().x / 3, secondary->get<CTransform>().pos.y + sRayCast.yOffset));
				}
				sRayCast.castRayEndPoints = addPoints;
			}
			else if (secondary->tag() == "ShipIdle")
			{
				std::vector<Vec2f> addPoints;
				addPoints.push_back(Vec2f(secondary->get<CTransform>().pos.x - sRayCast.xOffset, secondary->get<CTransform>().pos.y + m_game.window().getSize().y));
				sRayCast.castRayEndPoints = addPoints;
			}

			for (auto& castPoints : sRayCast.castRayEndPoints)
			{
				for (auto& polygonLine : worldPolygonLines)
				{
					auto checkInter = Utilis::LineIntersect(
						Vec2f(secondary->get<CTransform>().pos.x + sRayCast.xOffset, secondary->get<CTransform>().pos.y + sRayCast.yOffset),
						Vec2f(castPoints.x, castPoints.y),
						polygonLine.first,
						polygonLine.second);

					//If the collision result is true and there is a collision, apply logic based on entity
					if (checkInter.result && secondary->tag() == "spinShroom" && secondary->get<CState>().state != States::death &&  secondary->get<CState>().state != States::walk && secondary->get<CState>().state != States::hurt && secondary->get<CState>().state != States::longDead && primary->tag() == "Player")
					{
						
						secondary->get<CState>().state = States::walk;
						if (checkInter.pos.x > secondary->get<CTransform>().pos.x) { sRayCast.direction = "Right"; }
						else if (checkInter.pos.x < secondary->get<CTransform>().pos.x) { sRayCast.direction = "Left"; }
						sRayCast.canTurnAround = false;
					}
					else if (secondary->get<CState>().state != States::walk)
					{
						sRayCast.direction = "None";
						sRayCast.canTurnAround = true;
					}

					if (checkInter.result && secondary->tag() == "ShipIdle" && secondary->get<CState>().state != States::abduct && primary->tag() == "Player")
					{
						secondary->get<CState>().state = States::abduct;
					}

				}
			}

		}
	}
}

void Scene_Play::onEnd()
{
	//TODO: When the scene ends, change back to the MENU scene
	//      use m_game.changeScene(correct params);
}

void Scene_Play::sGUI()
{
	//ImGui::Begin("Scene Properties");

	//if (ImGui::BeginTabBar("My Tab Bar"))
	//{
	//	if (ImGui::BeginTabItem("Actions"))
	//	{
	//		for (const auto& [key, name] : getActionMap())
	//		{
	//			std::string ss = "START##" + name;
	//			std::string se = "END##" + name;

	//			if (ImGui::Button(ss.c_str()))
	//			{
	//				sDoAction(Action(name, "START"));
	//			}
	//			ImGui::SameLine();
	//			if (ImGui::Button(se.c_str()))
	//			{
	//				sDoAction(Action(name, "END"));
	//			}
	//			ImGui::SameLine();
	//			ImGui::Text("%s", name.c_str());
	//		}
	//		ImGui::EndTabItem();
	//	}

	//	if (ImGui::BeginTabItem("Assets"))
	//	{
	//		
	//	}
	//	ImGui::EndTabBar();
	//}
	//ImGui::End();
}

void Scene_Play::sRender()
{
	m_game.window().clear(sf::Color(100, 100, 255));

	auto distTileFromOriginalY = m_game.window().getSize().y - m_game.window().getDefaultView().getSize().y;
	auto playerPos = player()->get<CTransform>().pos;

	// Calculate window center based on player position
	float windowCenterX = std::max(m_game.window().getSize().x / 2.0f, playerPos.x);
	float windowCenterY = playerPos.y; // Follow player's Y position
	float m_playerViewYOffset = 128;

	sf::View mainView = m_game.window().getView();
	mainView.setCenter(windowCenterX, windowCenterY - distTileFromOriginalY / 2.0f - m_playerViewYOffset);
	m_game.window().setView(mainView);

	m_windowDifferenceY = m_game.window().getSize().y - m_game.window().getDefaultView().getSize().y;

	//TRY AND MAKE A THING THAT DOES UI STUFF USING THE COMPONENTS AND HOW THEY USE THE UI!!!

	// Calculate the top-left corner of the view
	sf::Vector2f viewTopLeft = sf::Vector2f(mainView.getCenter().x - mainView.getSize().x / 2.0f, mainView.getCenter().y - mainView.getSize().y / 2.0f);
	float offset = 0;
	for (auto& e : m_entityManager.getEntities("playerHeart"))
	{

		auto& eAnim = e->get<CAnimation>();
		// Position hearts relative to the view's top-left corner
		e->get<CTransform>().pos.y = viewTopLeft.y + 20 + eAnim.animation.getSprite().getTexture()->getSize().y / 2.0f;
		e->get<CTransform>().pos.x = viewTopLeft.x + offset + eAnim.animation.getSprite().getTexture()->getSize().x / 2.0f / eAnim.animation.getFrameCount();
		offset += 64;
	}

	if (m_drawTextures)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->tag() == "lvl1BG" || e->tag() == "MainMenuBG"  || e->tag() == "MercuryBG" || e->tag() == "VenusBG")
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
					sprite.setScale({scale.x, scale.y });
					sprite.setPosition({ transform.x, transform.y });
					if (e->isActive()) { m_game.window().draw(sprite); };
				}
			}
		}
		for (auto& textEntity : m_entityManager.getEntities("Text"))
		{
			if (textEntity->has<CText>())
			{
				textEntity->get<CText>().text.setPosition({ textEntity->get<CTransform>().pos.x, textEntity->get<CTransform>().pos.y });
				m_game.window().draw(textEntity->get<CText>().text);
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

	////draw the grid for debugging
	if (m_drawGrid)
	{
		drawGrid();
	}


	for (auto e : m_entityManager.getEntities())
	{
		// In sMovement() or sRender()
		if (e->has<CRayCast>() )
		{
			auto& transform = e->get<CTransform>().pos;
			auto& bbox = e->get<CBoundingBox>();
			auto& rayCast = e->get<CRayCast>();

			// Update collision shape points
			rayCast.collisionShape.setPoint(0, { -bbox.halfsize.x, -bbox.halfsize.y });
			rayCast.collisionShape.setPoint(1, { -bbox.halfsize.x,  bbox.halfsize.y });
			rayCast.collisionShape.setPoint(2, { bbox.halfsize.x,  bbox.halfsize.y });
			rayCast.collisionShape.setPoint(3, { bbox.halfsize.x, -bbox.halfsize.y });
			rayCast.collisionShape.setOutlineColor(sf::Color::Magenta);
			rayCast.collisionShape.setOutlineThickness(2);
			rayCast.collisionShape.setFillColor(sf::Color::Transparent);

			// Update position
			rayCast.collisionShape.setPosition(transform.x, transform.y);

			if (m_drawRaycast)
			{
				//Draw the rays as well
				for (auto& point : rayCast.castRayEndPoints)
				{
					sf::VertexArray ray(sf::Lines, 2);
					ray[0].position = sf::Vector2f(transform.x + rayCast.xOffset, transform.y + rayCast.yOffset);
					ray[1].position = sf::Vector2f(point.x, point.y);
					ray[0].color = sf::Color::Red;
					ray[1].color = sf::Color::Red;
					m_game.window().draw(ray);
				}


				m_game.window().draw(rayCast.collisionShape);
			}

		}
		
	}
	
	if (m_showFPS)
	{
		m_game.getFPS().setPosition({ viewTopLeft.x, viewTopLeft.y });
		m_game.window().draw(m_game.getFPS());
	}


	
	m_game.window().display();
}

std::shared_ptr<Entity> Scene_Play::player()
{
	auto& players = m_entityManager.getEntities("Player");
	return players.front();
}

void Scene_Play::drawGrid()
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

const std::string Scene_Play::checkTypeOfTile(const std::string& tag)
{
	std::string baseCase = "Tile";
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

bool Scene_Play::StillPushing(std::shared_ptr<Entity> player, std::shared_ptr<Entity> block) const
{
	auto& playerPos = player->get<CTransform>();
	auto& blockPos = block->get<CTransform>();
	//this if statement is incase I want to have a m_currentCarry also;
	if (block == m_currentPush)
	{
		if (blockPos.pos.x > playerPos.pos.x && playerPos.velocity.x < 0)
		{
			return false;
		}
		else if (blockPos.pos.x < playerPos.pos.x && playerPos.velocity.x > 0)
		{
			return false;
		}
	
		if (playerPos.velocity.y > m_playerConfig.GRAVITY)
		{
			
			return false;
		}
		if (playerPos.velocity.y < 0) //put here here in case of gravity
		{
	
			return false;
		}
	}
	return true;
}

void Scene_Play::putDownBlock()
{
	float blockyPos = m_currentHold->get<CTransform>().pos.y;

	m_currentHold->get<CTransform>().pos.y = player()->get<CTransform>().pos.y;
	player()->get<CTransform>().pos.y = blockyPos;

	m_currentHold = nullptr;
	m_currentPush = nullptr;
	player()->get<CState>().state = States::holdingBox;
}

void Scene_Play::spawnDustCloud(std::shared_ptr<Entity> entity)
{
	Vec2f& entityPos = entity->get<CTransform>().pos;

	auto dustCloud = m_entityManager.addEntity("DustCloud");
	dustCloud->add<CTransform>(entityPos, Vec2f(0, 0), Vec2f(2, 2), 0);
	dustCloud->add<CBoundingBox>(Vec2f(0, 0));

	if (m_entityManager.getEntities("attackBox").size() > 1)
	{
		m_entityManager.getEntities("attackBox").back()->destroy();
	}

	if (entity->tag() == "Player" && player()->get<CInput>().jump)
	{
		dustCloud->add<CAnimation>(m_game.assets().getAnimation("JumpParticle"), false);
	}
	else if (checkTypeOfTile(entity->tag()) == "MoveAble")
	{
		dustCloud->add<CAnimation>(m_game.assets().getAnimation("BlockFallingP"), false);
	}
	else
	{
		dustCloud->destroy();
	}
}

