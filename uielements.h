#pragma once
#include "box2.h"
#include "window.h"
#include "cstring"
#include <functional>

class window;

class sprite {
	sf::Texture tex;
	sf::Sprite spr;
	sf::Vector2u s0;
public:
	sprite(std::string src, sf::Vector2u gridSize = {1, 1}, uint32_t id = 0);
	void draw(window* w, box2 zone);
};

enum class scaleMode {
	bindBL,
	bindTR,
	bindTL,
	bindBR,
	scaleXY
};

enum class mouseEvent {
	pressing,
	holding,
	release
};

class uiElement {
protected:
	scaleMode sm;
	box2 boxOrigin, boxScaled;

	uiElement(box2 zone, scaleMode sm);
public:
	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled);
	void reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);
	virtual void draw(window*) = 0;
	virtual void onClick(sf::Vector2f pos, mouseEvent event);
};

class uiImage : public uiElement {
protected:
	sprite spr;
	void draw(window*) override;
public:
	uiImage(box2 zone, scaleMode sm, std::string src);
};

class uiTilemap : public uiElement {
	uint32_t* map;
	sf::Vector2u srcGridSize, gridSize;
	sprite** sprites;

	void draw(window*) override;
	size_t size() const;
public:
	uiTilemap(box2 zone, scaleMode sm, std::string src, sf::Vector2u srcGridSize);
	~uiTilemap();
	void setIndexes(const uint32_t* map, sf::Vector2u _gridSize);
};

class uiButton : public uiElement {
	bool isPressed;
	sprite sprf, sprp;
	std::function<void()> action;

	void draw(window*) override;
	void onClick(sf::Vector2f pos, mouseEvent event) override ;
public:
	uiButton(box2 zone, scaleMode sm, std::string srcFree, std::string srcPressed, std::function<void()> action);
};
