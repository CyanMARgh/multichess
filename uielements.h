#pragma once
#include "box2.h"
#include "window.h"
#include "cstring"
#include <functional>
#include <cstdint>
#include <unordered_set>
#include <memory>

class window;
struct spriteparam {
	std::string s;
	sf::Vector2u gm = {1, 1};
	int i = 0;
};
struct sprbase {
	std::string name;
	sf::Texture tex;
	sf::Sprite spr;
	sf::Vector2u s0;
	sprbase(const sf::Image& img, const std::string& name, sf::IntRect rect);
	sprbase& operator=(const sprbase&) = delete;
	void draw(window* w, box2 zone);
	static std::unordered_set<std::shared_ptr<sprbase>> loadedSprites;
};
class sprite {
	std::shared_ptr<sprbase> sbptr;
public:
	static void loadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics);
	sprite();
	explicit sprite(const std::string& src, sf::Vector2u gridMetrics, uint32_t id);
	explicit sprite(const spriteparam& par);
	void draw(window* w, box2 zone);
};
class spriteStr {
	//
};

class textSprite {
	sf::Font font;
	sf::Text text;
	std::string stext;
public:
	textSprite(const std::string& fontSrc, const std::string& textSrc);
	void setText(const std::string& data);
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
	bool visible = true;
	bool clickable = false;
	bool pressed = false;

	bool isClickable() const;
	bool isVisible() const;
	sf::Vector2f toUnit(sf::Vector2f pos) const;

	bool isInside(sf::Vector2f pos) const;
	virtual void reshape(box2 parentBoxOrigin, box2 parentBoxScaled);
	void reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);
	virtual void draw(window* w);

	const int NO_HIT = -2, HIT_NO_ACTION = -1;
	virtual int onMouseEvent(mouseEvent event, sf::Vector2f pos);
};
class uiGroup : public uiElement {
	std::vector<std::unique_ptr<uiElement>> parts;
	int pressedPart = -1;

	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled) final;
protected:
	void draw(window* w) override;
	int onMouseEvent(mouseEvent event, sf::Vector2f pos) override;
	virtual box2 getSubBox(uint i);
public:
	explicit uiGroup(box2 zone = box2::unit(), scaleMode sm = scaleMode::fullZone, uint count = 0);
	void addUIPart(uiElement* uiel);
	void setPart(uiElement* uiel, uint32_t id);
};

class uiImage final : public uiElement {
protected:
	sprite spr;
	void draw(window* w) final;
public:
	uiImage(box2 zone, scaleMode sm, const spriteparam& src);
};
class uiText final : public uiElement {
protected:
	textSprite spr;
	void draw(window* w) final;
public:
	uiText(box2 zone, scaleMode sm, const std::string& textSrc, const std::string& fontSrc);
	void setString(const std::string& s);
};
class uiTilemap final : public uiElement {
	std::vector<uint32_t> map;
	sf::Vector2u gridSize;
	std::vector<sprite> tilemap;

	void draw(window* w) final;
	size_t size() const;
public:
	uiTilemap(box2 zone, scaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg = 0, uint32_t end = -1);
	void setIndexes(std::vector<uint32_t> map, sf::Vector2u _gridSize);
	void setByIndex(uint32_t cellId, uint32_t texId);
};
class uiInvisibleButton : public uiElement {
	uint32_t id;
protected:
	int onMouseEvent(mouseEvent event, sf::Vector2f pos) override;
public:
	uiInvisibleButton(box2 zone, scaleMode sm, uint32_t id);
};
class uiButton final : public uiInvisibleButton {
	sprite sprf, sprp;
protected:
	void draw(window* w) override;
public:
	uiButton(box2 zone, scaleMode sm, const spriteparam& parFree, const spriteparam& parPressed, uint32_t id);
};

//class uiSelectable;
//class selector {
//	int selected;
//	std::vector<uiSelectable*> variants;
//public:
//	explicit selector(uint count);
//	void edit(uint id);
//	void setVariant(uiSelectable& el, uint id);
//	int getSelected() const;
//};
//class uiSelectable : public uiButton {
//	sprite* selSprite;
//	bool isSelected;
//	void draw(window* w) override;
//public:
//	uiSelectable(box2 zone, scaleMode sm, sprite* selSprite, selector& sel, uint id, const spriteparam& srcFree, const spriteparam& srcPressed);
//	void setSelection(bool mode);
//};

class uiSideCut : public uiGroup {
	box2 getSubBox(uint i) final;
	sf::Vector2f subSize;
public:
	explicit uiSideCut(sf::Vector2f subSize, box2 zone = box2::unit(), scaleMode sm = scaleMode::fullZone);
};
class uiGrid : public uiGroup {
	box2 getSubBox(uint i) final;
	sf::Vector2u metrics;
public:
	explicit uiGrid(sf::Vector2u metrics, box2 zone = box2::unit(), scaleMode sm = scaleMode::fullZone);
};
