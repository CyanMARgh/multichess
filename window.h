#pragma once
#include <map>
#include <vector>
#include "box2.h"
#include <SFML/Graphics.hpp>
#include "uielements.h"
#include <functional>

class uiElement;

class window {
	float minSize, minQ;
	bool hv;

	uiElement* scene = nullptr;
	sf::RenderWindow rw;
	sf::Vector2f sizeOrigin, sizeScaled;
	sf::Vector2f getActualSize(sf::Vector2f s) const;
	std::string name;

	std::vector<std::function<void(uint32_t)>> keyEvents;
	//std::vector<std::function<void()>> updateEvents;

public:
	void drawAll();

	window(const std::string& name, sf::Vector2f size, float minSize = 0, float minQ = 1, bool hv = true);
	void startRenderCycle();

	void setUIScene(uiElement& uiel);
	void addKeyEvent(const std::function<void(uint32_t)>& event);
	//void addUpdateEvent(const std::function<void()>& event);


	friend class sprbase;
	friend class textSprite;
};