#pragma once
#include <map>
#include <vector>
#include "box2.h"
#include <SFML/Graphics.hpp>
#include "uielements.h"
#include <functional>
#include <cassert>
#include <thread>

class uiElement;
class appManager;

class window {
	// исправить на что-то вменяемое, если понадобится
	enum state_t {
		PREPARE,
		BLOCKED,
		RUN,
		STOP,
		ERROR
	} state;

	std::thread renderThread;
	std::vector<uiElement*> scenes;
	uint32_t currentScene = 0;
	appManager* manager = nullptr;

	sf::RenderWindow rw;
	sf::Vector2f sizeOrigin, sizeScaled;
	std::string name;
public:
	void refresh();

	void renderCycle();
	void startRenderCycle();

	void setScene(uiElement& uiel, uint32_t id);
	void setManager(appManager& manager);
	void switchScene(uint32_t sceneId);

	window(const std::string& name, sf::Vector2f size, uint32_t sceneCount);
	void wait();

	friend class sprbase;
	friend class textSprite;
	friend class appManager;
};

class appManager {
protected:
	uint32_t currentScene = 0;
	std::vector<uiElement*> uiels;
	window* w;
public:
	virtual void onKeyEvent(uint32_t keyCode);
	virtual void onSceneSwitch(uint32_t sceneCode);
	virtual void onBtnClick(uint32_t btnId, sf::Vector2f pos);
	virtual void onExit();
	void unblock();

	void switchScene(uint32_t sceneId);
	void setUiPartsPtrs(std::vector<uiElement*> uiels);
	explicit appManager(window& w);
};
