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
	scaleMode sm;
	box2 boxOrigin, boxScaled;

	uiElement(box2 zone, scaleMode sm);
public:
	virtual void reshape(box2 parentBoxOrigin, box2 parentBoxScaled);
	void reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);
	virtual void draw(window* w) = 0;
	virtual bool onClick(sf::Vector2f pos, mouseEvent event);
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
	bool onClick(sf::Vector2f pos, mouseEvent event) final;
public:
	uiButton(box2 zone, scaleMode sm, const std::string& srcFree, const std::string& srcPressed, std::function<void()> action);
};

class uiScene : public uiElement {
	bool visible, clickable;
	std::vector<uiElement*> parts;
//	std::vector<uiScene> subscenes;
//	uint subscene = -1;

	void draw(window* w) final;
	bool onClick(sf::Vector2f pos, mouseEvent event) final;
	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled) final;
public:
	explicit uiScene(bool isActive, box2 zone = {0, 0, 1, 1});
	void addUIPart(uiElement& uiel);
	void changeToNewScene(uiScene* other);
//	void openSubScene(uint id);
//	void closeSubScene();
//	void close();
};

class uiSlicer : public uiElement {
	std::vector<uiElement*> parts;

	void draw(window* w) final;
	bool onClick(sf::Vector2f pos, mouseEvent event) final;
	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled) override;

	virtual box2 getSubBox(uint i) = 0;
protected:
	explicit uiSlicer(int partsCount, box2 zone = {0, 0, 1, 1}, scaleMode sm = scaleMode::fullZone);
public:
	void setUIPart(uint i, uiElement& uiel);
};

class uiSideCut : public uiSlicer {
	box2 getSubBox(uint i) final;
	sf::Vector2f subSize;
public:
	explicit uiSideCut(sf::Vector2f subSize, box2 zone = {0, 0, 1, 1}, scaleMode sm = scaleMode::fullZone);
};
