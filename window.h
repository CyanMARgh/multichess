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
	std::vector<uiElement*> ordered;
	uint32_t currentScene = 0;
	appManager* manager = nullptr;

	sf::RenderWindow rw;
	sf::Vector2f sizeOrigin, sizeScaled;
	std::string name;
public:
	void refresh(bool check);

	void renderCycle();
	void startRenderCycle();

	void setScene(uiElement* uiel, uint32_t id);
	void setManager(appManager& manager);
	void switchScene(uint32_t sceneId);

	window(const std::string& name, sf::Vector2f size, uint32_t sceneCount);
	~window();
	void wait();

	friend class sprbase;
	friend class textSprite;
	friend class shaderSprite;
	friend class appManager;
};

class appManager {
protected:
public:
	uint32_t currentScene = 0;
	window* w;

	virtual void onKeyEvent(uint32_t keyCode);
	virtual void onSceneSwitch(uint32_t sceneCode);
	virtual void onBtnClick(uint32_t btnId, sf::Vector2f pos);
	virtual void onExit();
	void unblock();

	void switchScene(uint32_t sceneId);
	explicit appManager(window& w);
};
class appManagerDefault : public appManager {
	typedef std::function<void(uint32_t, sf::Vector2f, appManager*)> obc_t;
	typedef std::function<void(uint32_t, appManager*)> oke_t, oss_t;
	typedef std::function<void(appManager*)> oe_t;

	void onKeyEvent(uint32_t keyCode) override;
	void onSceneSwitch(uint32_t sceneCode) override;
	void onBtnClick(uint32_t id, sf::Vector2f pos) override;
	void onExit() override;
public:
	obc_t obc = [](uint32_t, sf::Vector2f, appManager* am) { am->unblock(); };
	oke_t oke = [](uint32_t, appManager*) { };
	oss_t oss = [](uint32_t, appManager*) { };
	oe_t oe = [](appManager* am) { am->appManager::onExit(); };

	explicit appManagerDefault(window& w);
};
