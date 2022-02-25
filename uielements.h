#pragma once
#include "box2.h"
#include "window.h"
#include "cstring"

enum class scaleMode {
	bindBL,
	bindTR,
	bindTL,
	bindBR,
	scaleXY
};

class window;

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
	void draw(window*) override;
public:
	uiImage(box2 zone, scaleMode sm, std::string src);
};

class uiTilemap : public  uiElement {
	std::string src;
	uint32_t* map;
	sf::Vector2u count, metrics;
public:
	uiTilemap(box2 zone, scaleMode sm, std::string src, sf::Vector2u count);
	void draw(window*) override;
	void setIndexes(const uint32_t* map, sf::Vector2u metrics);
	size_t size() const;
};
