#pragma once
#include "box2.h"
#include "window.h"
#include "cstring"
#include <functional>
#include <cstdint>

class window;
class sprite {
	sf::Texture tex;
	sf::Sprite spr;
	sf::Vector2u s0;
public:
	explicit sprite(const std::string& src, sf::Vector2u gridSize = {1, 1}, uint32_t id = 0);
	void draw(window* w, box2 zone);
};
enum class scaleMode {
	bindBL,
	bindTR,
	bindTL,
	bindBR,
	scaleXY,
	fitCenter,
	fullZone
};
enum class mouseEvent {
	pressing,
	holding,
	release
};

class uiElement {
protected:
	bool visible = true;
	scaleMode sm;
	box2 boxOrigin, boxScaled;

	uiElement(box2 zone, scaleMode sm);
public:
	uiElement& operator=(const uiElement&) = delete;
	uiElement(const uiElement&) = delete;

	bool isInside(sf::Vector2f pos) const;
	virtual void reshape(box2 parentBoxOrigin, box2 parentBoxScaled);
	void reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);
	virtual void draw(window* w) = 0;
	virtual bool onMouseEvent(sf::Vector2f pos, mouseEvent event);
};
class uiGroup : public uiElement {
	std::vector<uiElement*> parts;
	int pressedPart = -1;

	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled) final;
protected:
	void draw(window* w) override;
	bool onMouseEvent(sf::Vector2f pos, mouseEvent event) override;
	virtual box2 getSubBox(uint i);
public:
	explicit uiGroup(box2 zone = box2::unit(), scaleMode sm = scaleMode::fullZone, uint count = 0);
	void addUIPart(uiElement& uiel);
	void setUIPart(uint i, uiElement& uiel);
};

class uiImage : public uiElement {
protected:
	sprite spr;
	void draw(window* w) final;
public:
	uiImage(box2 zone, scaleMode sm, const std::string& src);
};
class uiTilemap : public uiElement {
	uint32_t* map;
	sf::Vector2u srcGridSize, gridSize;
	sprite** sprites;

	void draw(window* w) final;
	size_t size() const;
public:
	uiTilemap(box2 zone, scaleMode sm, const std::string& src, sf::Vector2u srcGridSize);
	~uiTilemap();
	void setIndexes(const uint32_t* map, sf::Vector2u _gridSize);
};
class uiButton : public uiElement {
	bool isPressed;
	sprite sprf, sprp;
	std::function<void()> action;

	void draw(window* w) final;
	bool onMouseEvent(sf::Vector2f pos, mouseEvent event) final;
public:
	uiButton(box2 zone, scaleMode sm, const std::string& srcFree, const std::string& srcPressed, std::function<void()> action);
};

class uiScene : public uiGroup {
public:
	explicit uiScene(bool isActive, box2 zone = box2::unit());
	void changeToNewScene(uiScene* other);
};

class uiSideCut : public uiGroup {
	box2 getSubBox(uint i) final;
	sf::Vector2f subSize;
public:
	explicit uiSideCut(sf::Vector2f subSize, box2 zone = box2::unit(), scaleMode sm = scaleMode::fullZone);
};
