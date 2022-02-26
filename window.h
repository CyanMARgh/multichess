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
public:
	void drawAll();

	window(const std::string& name, sf::Vector2f size);
	void addUIPart(uiElement* uielptr);
	void startRenderCycle();

	friend class sprite;
};