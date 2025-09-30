//DONE

#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>

template <typename T>
//The Vector Two class of the game 
class Vec2
{
public:
	//The class holds these two x and y portions (Type T because they might be either floats or ints)
	T x = 0;
	T y = 0;

	Vec2() = default; //The default constructor (specifcally told to the IDE)

	//The argument constructor that assigns both x and y
	Vec2(T xin, T yIn)
		: x(xin)
		, y(yIn)
	{
	}

	//Constructor to convert from sfml Vec2 class to my Vec2 class
	Vec2(const::sf::Vector2<T>& vec)
		: x(vec.x)
		, y(vec.y)
	{
	}

	//allow automatic conversion from Vec to to sf::Vector2 (I dont think this works in sfml 3.0.0)
	//
	//operator sf::Vector2<T>();
	//{
	//	return sf::Vector2<T>(x, y);
	//}

	//----------------------------------------------------------------------------------------------
	//VECTOR MATH COMPONENTS OF CLASS
	//----------------------------------------------------------------------------------------------
	Vec2 operator + (const Vec2& rhs) const
	{
		Vec2 vec;
		vec.x = x + rhs.x;
		vec.y = y + rhs.y;
		return vec;
	}
	Vec2 operator - (const Vec2& rhs) const
	{
		Vec2 vec;
		vec.x = x - rhs.x;
		vec.y = y - rhs.y;
		return vec;
	}
	Vec2 operator * (const T val) const
	{
		Vec2 vec;
		vec.x = x * val;
		vec.y = y * val;
		return vec;
	}
	Vec2 operator / (const T val) const
	{
		Vec2 vec;
		vec.x = x / val;
		vec.y = y / val;
		return vec;
	}

	bool operator == (const Vec2& rhs) const
	{
		return x == rhs.x and y == rhs.y;
	}

	bool operator != (const Vec2& rhs) const
	{
		return x != rhs.x or y != rhs.y;
	}

	void operator += (const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}
	void operator -= (const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}
	void operator *= (const T val)
	{
		x *= val;
		y *= val;
	}
	void operator /= (const T val)
	{
		x /= val;
		y /= val;
	}
	float dist(const Vec2& rhs) const
	{
		return sqrtf((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));  // Add implementation
	}
};

using Vec2f = Vec2<float>;
