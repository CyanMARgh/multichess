#pragma once
#include <map>
#include <vector>
#include "box2.h"
#include "SFML/Graphics.hpp"
#include "uielements.h"

class uiElement;

class window {
	std::vector<uiElement*> parts;
	sf::RenderWindow rw;
	sf::Vector2f sizeOrigin, sizeScaled;
	std::map<std::string, sf::Texture> loadedTextures;
public:
	void drawAll();
	void drawText();
	void drawImage(std::string src, box2 box, sf::Vector2u gridSize = {1, 1}, uint32_t id = 0);

	window(const std::string& name, sf::Vector2f size);
	void addUIPart(uiElement* uielptr);
	void startRenderCycle();
};