#pragma once
#include "box2.h"
#include "window.h"
#include "cstring"

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

class uiElement {
protected:
	scaleMode sm;
	box2 boxOrigin, boxScaled;

	uiElement(box2 zone, scaleMode sm);
public:
	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled);
	void reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);
	virtual void draw(window*) = 0;
};

class uiImage : public uiElement {
protected:
	std::string src;
	sprite spr;
	void draw(window*) override; // -> call spr.draw
public:
	uiImage(box2 zone, scaleMode sm, std::string src);
};

class uiTilemap : public uiElement {
	std::string src;
	uint32_t* map;
	sf::Vector2u srcGridSize, gridSize;
	sprite** sprites;
public:
	uiTilemap(box2 zone, scaleMode sm, std::string src, sf::Vector2u srcGridSize);
	~uiTilemap();
	void draw(window*) override; // -> call spr.draw
	void setIndexes(const uint32_t* map, sf::Vector2u _gridSize);
	size_t size() const;
};
