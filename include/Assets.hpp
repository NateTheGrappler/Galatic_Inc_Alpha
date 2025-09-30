//VIDEO DIDNT GO UP THIS HIGH EITHER, KEEP IN MIND WHEN CODING (otherwise done)
#pragma once
#include "Animation.hpp"
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <fstream>

class Assets
{

	std::map<std::string, sf::Texture>  m_textureMap;
	std::map<std::string, Animation>    m_animationMap;
	std::map<std::string, sf::Font>     m_fontMap;

	void addTexture(const std::string& textureName, const std::string& path)
	{
		m_textureMap[textureName] = sf::Texture();

		if (!m_textureMap[textureName].loadFromFile(std::string(RESOURCES_PATH) + path))
		{
			std::cerr << "Could not load texture file: " << path << std::endl;
			m_textureMap.erase(textureName);
		}
		else
		{
			m_textureMap[textureName].setSmooth(false);
			std::cout << "Loaded Texture: " << path << std::endl;
		}
	}

	void addAnimation(const std::string& animationName, const sf::Texture& t, size_t frameCount, size_t speed) //the rest of it is cut off by the video, refrence the animation file for clues when done
	{
		m_animationMap[animationName] = Animation(animationName, t, frameCount, speed);
	}


	void addFont(const std::string& fontName, const std::string& path)
	{
		m_fontMap[fontName] = sf::Font();
		if (!m_fontMap[fontName].loadFromFile(std::string(RESOURCES_PATH) + path))
		{
			std::cerr << "Could not load font file: " << path << std::endl;
			m_fontMap.erase(fontName);
		}
		else
		{
			std::cout << "Loaded Font     " << path << std::endl;
		}
	}
public:
	Assets() = default;

	void loadFromFile(const std::string& path)
	{
		std::string fullPath = std::string(RESOURCES_PATH) + "assets/" + path;
		std::cout << "Loading assets from: " << fullPath << std::endl;

		std::ifstream file(fullPath);

		if (!file.is_open()) {
			std::cerr << "ERROR: Could not open assets file: " << fullPath << std::endl;
			return;
		}

		std::string str;
		while (file.good())
		{
			file >> str;
			if (str == "Texture")
			{
				std::string name, path;
				file >> name >> path;
				addTexture(name, path);
			}
			else if (str == "Animation")
			{
				std::string name, textureName;
				size_t frames, speed;
				file >> name >> textureName >> frames >> speed;
				addAnimation(name, getTexture(textureName), frames, speed);
			}
			else if (str == "Font")
			{
				std::string name, path;
				file >> name >> path;
				addFont(name, path);
			}
			else
			{
				std::cerr << "Unkown Asset Type." << str << std::endl;
			}
		}
	}


	const sf::Texture& getTexture(const std::string& textureName) const
	{
		assert(m_textureMap.find(textureName) != m_textureMap.end());
		return m_textureMap.at(textureName);
	}

	const Animation& getAnimation(const std::string& animationName) const
	{
		assert(m_animationMap.find(animationName) != m_animationMap.end());
		return m_animationMap.at(animationName);
	}

	const sf::Font& getFont(const std::string& fontName) const
	{
		assert(m_fontMap.find(fontName) != m_fontMap.end());
		return m_fontMap.at(fontName);
	}

	const std::map<std::string, sf::Texture>& getTextures() const
	{
		return m_textureMap;
	}

	std::map<std::string, Animation>& getAnimations()
	{
		return m_animationMap;
	}
};