//NOT DONE, CHECK LECTURE FROM 2022
#pragma once
#include "Vec2.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Animation
{
	sf::Sprite  m_sprite;
	size_t      m_frameCount = 1; //Total number of frames of animation
	size_t      m_gameFrames = 0; //The number of games
	size_t      m_framesPast = 0;
	size_t      m_currentFrame = 0; // the currenct frame of animation being played
	size_t      m_speed = 0; // the speed to play this animation per each one of the frames
	Vec2f       m_size = { 1, 1 }; //The size of the animation frame
	std::string m_name = "NONE";

public:
	Animation() = default;

	Animation(const std::string& name, const sf::Texture& t)
		: Animation(name, t, 1, 0)
	{
	}

	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
		: m_name(name)
		, m_sprite(t)
		, m_frameCount(frameCount)
		, m_currentFrame(0)
		, m_speed(speed)
	{
		m_size = Vec2f((float)t.getSize().x / frameCount, (float)t.getSize().y);
		m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
		m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
	}

	//updates the animation to show the next frame, depending on its speed
	// animation loops when it reaches the end
	void update()
	{
		// add the speed varaible to the current frame;
		m_gameFrames++;
		if (m_gameFrames >= m_speed)
		{
			m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_size.x, 0, m_size.x, m_size.y));
			m_currentFrame++;
			m_framesPast = m_currentFrame;
			if (m_currentFrame == m_frameCount)
			{
				m_currentFrame = 0;
			}
			m_gameFrames = 0;
		}

		//TODO: 1) Calculate the correct frame of animation to play based on currentFrame and speed
		//      2) set the texture rectangle properly (see constructor for sample)
	}

	bool hasEnded()
	{
		// TODO: detect when animaton has ended (last frame was played) and return true
		if (m_framesPast == m_frameCount)
		{
			return true;
		}
		return false;
	}

	const Vec2f& getSize() const
	{
		return m_size;
	}

	const std::string& getName() const
	{
		return m_name;
	}

	sf::Sprite& getSprite()
	{
		return m_sprite;
	}
	size_t getFrameCount()
	{
		return m_frameCount;
	}

};