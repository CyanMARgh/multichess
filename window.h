#pragma once
#include <map>
#include <vector>
#include "box2.h"
#include "SFML/Graphics.hpp"
#include "uielements.h"

class uiElement;

class window {
	float minSize, minQ;
	bool hv;

	uiElement* scene = nullptr;
	sf::RenderWindow rw;
	sf::Vector2f sizeOrigin, sizeScaled;
	sf::Vector2f getActualSize(sf::Vector2f s) const;
public:
	void drawAll();

	window(const std::string& name, sf::Vector2f size, float minSize = 0, float minQ = 1, bool hv = true);
	void setUIScene(uiElement& uiel);
	void startRenderCycle();

	friend class sprite;
};