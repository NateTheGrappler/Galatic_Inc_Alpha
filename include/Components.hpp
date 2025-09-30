//DONE
#pragma once
#include "Animation.hpp"
#include "Assets.hpp"
#include <vector>
#include "GameEngine.h"

class Component
{
public:
	bool exists = false;

	Component() = default;
	virtual ~Component() = default;
};

class CTransform : public Component
{
public:
	Vec2f pos      = { 0.0, 0.0 };
	Vec2f ogPos    = { 0.0f, 0.0f };
	Vec2f prevPos  = { 0.0, 0.0 };
	Vec2f scale    = { 1.0, 1.0 };
	Vec2f ogScale  = { 1.0, 1.0 };
	Vec2f velocity = { 0.0, 0.0 };
	float angle    = 0;

	CTransform() = default;

	CTransform(const Vec2f& p)
		:pos(p) {}

	CTransform(const Vec2f& position, const Vec2f& velocity, const Vec2f& scale, float angle) // Fixed typo in constructor name and added missing '&' for velocity parameter
		: pos(position)
		, ogPos(position)
		, prevPos(position)
		, velocity(velocity)
		, scale(scale)
		, ogScale(scale)
		, angle(angle)
	{}
};

class CLifespan : public Component
{
public:
	int lifeSpan = 0;
	int frameCreated = 0;

	CLifespan() = default;

	CLifespan(int duration, int frame)
		: lifeSpan(duration)
		, frameCreated(frame)
	{}
};
class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool jump = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() = default;
};

class CMovePoints : public Component
{
public:
	std::vector<size_t> entityPointerVector;
	int movePointCount = 0;

	CMovePoints() = default;

	CMovePoints(int movePointCount)
		: movePointCount(movePointCount)
	{ 
	}
};

class CBoundingBox : public Component
{
public:
	Vec2f size;
	Vec2f halfsize;
	int iFrames = 0;

	CBoundingBox() = default;

	CBoundingBox(const Vec2f& size)
		: size(size)
		, halfsize(size.x / 2, size.y / 2)
	{
	}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;

	CAnimation() = default;
	CAnimation(const Animation& animation, bool repeat)
		: animation(animation)
		,repeat(repeat)
	{
	}
};
class CGravity : public Component
{
public:
	float gravity = 0;
	CGravity() = default;

	CGravity(float g) 
		: gravity(g)
	{
	}
};

class CState : public Component
{
public:
	States state = States::base;
	States previousState = States::base;
	ArialStates aerialState = ArialStates::base;
	std::string hostileState = "passive";
	bool isPushing = false;
	bool isEnemy = false;
	bool isClicked = false; //Strictly for level creation

	// Default constructor - FIXED
	CState() = default;

	// Constructor taking enum by value, not reference
	CState(const States initialState)
		: state(initialState)
		, previousState(initialState)
	{
	}

	// Constructor with enum and bool
	CState(const States initialState, bool enemy)
		: state(initialState)
		, previousState(initialState)
		, isEnemy(enemy)
	{
	}

	// Constructor just for isEnemy
	CState(bool enemy)
		: isEnemy(enemy)
	{
	}
};

class CText : public Component
{
public:
	sf::Text text;
	CText() = default;
	CText(const sf::Font& font)
	{
		text.setFont(font);
	}
};
class CHealth : public Component
{
public:
	float baseHealth;
	float currentHealth;
	int numOfHearts = 5;
	CHealth() = default;

	CHealth(const int numOfHearts)
		:numOfHearts(numOfHearts)
	{
	}
	CHealth(const float health)
		:baseHealth(health)
		,currentHealth(health)
	{ 
	}
};

class CUserInterface : public Component
{
public:
	bool isUI = false;
	CUserInterface() = default;
	CUserInterface(const bool isUI)
		:isUI(isUI)
	{
	}

};



class CRayCast : public Component {
public:
	sf::ConvexShape collisionShape;
	Vec2f castPoint = { 0, 0 };
	float pointCount = 0;
	std::vector<Vec2f> polygonPoints;
	std::vector<Vec2f> castRayEndPoints;
	float yOffset = 0;
	float xOffset = 0;
	float turnAroundCount = 60;
	bool canTurnAround = true;
	std::string direction = "None";

	CRayCast() 
	{
		collisionShape.setPointCount(0); 
	}

	explicit CRayCast(const Vec2f& castPoint)
		: castPoint(castPoint) 
	{
		collisionShape.setPointCount(0);
	}

	explicit CRayCast(const Vec2f& castPoint, float pointCount)
		: castPoint(castPoint)
		, pointCount(pointCount) 
	{
		collisionShape.setPointCount(pointCount);
	}

	explicit CRayCast(const Vec2f& castPoint, float pointCount, float xOffset, float yOffset)
		: castPoint(castPoint)
		, pointCount(pointCount)
		, xOffset(xOffset)
		, yOffset(yOffset)
	{
		collisionShape.setPointCount(pointCount);
	}
};