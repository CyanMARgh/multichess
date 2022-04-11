#include "window.h"

void appManager::onKeyEvent(uint32_t keyCode) { }
void appManager::onSceneSwitch(uint32_t sceneCode) { }
void appManager::onExit() {
	w->state = window::STOP;
}
void appManager::onBtnClick(uint32_t btnId, sf::Vector2f pos) {
	unblock();
}
void appManager::switchScene(uint32_t sceneId) {
	currentScene = sceneId;
	w->switchScene(sceneId);
}
void appManager::unblock() {
	w->state = window::RUN;
}
appManager::appManager(window& w) :w(&w), currentScene(0) { }

void appManagerDefault::onBtnClick(uint32_t id, sf::Vector2f pos) {
	_onBtnClick(id, pos, this);
}
appManagerDefault::appManagerDefault(window& w, obc_t _onBtnClick) :appManager(w), _onBtnClick(std::move(_onBtnClick)) { }

void window::refresh(bool check) {
	if (check) {
		for (auto* e : ordered) {
			if (!e->isFresh()) goto NOT_FRESH;
		}
		return;
	}
NOT_FRESH:
	for (auto* e : ordered) {
		e->setFresh(true);
	}
	for (auto* s : scenes) {
		if (s->isVisible())s->reshape(sizeOrigin, sizeScaled);
	}
	scenes[currentScene]->reshape(sizeOrigin, sizeScaled);
	rw.clear();
	scenes[currentScene]->draw(this);
	rw.display();
}
void window::setScene(uiElement* uiel, uint32_t id) {
	scenes[id] = uiel;
}
void window::setManager(appManager& _manager) {
	manager = &_manager;
}
void window::switchScene(uint32_t sceneId) {
	scenes[currentScene]->setVisible(false);
	scenes[currentScene]->setClickable(false);
	scenes[sceneId]->setVisible(true);
	scenes[sceneId]->setClickable(true);
	currentScene = sceneId;
	refresh(false);
}

void window::renderCycle() {
	rw.create(sf::VideoMode((uint)sizeScaled.x, (uint)sizeScaled.y), name);
	for (int i = 0; i < scenes.size(); i++) {
		assert(scenes[i]);
		scenes[i]->setVisible(!i);
		scenes[i]->setClickable(!i);
		scenes[i]->addPartsOrdered(ordered);
	}
	assert(manager);
	bool pressed = false;

	refresh(false);
	state = RUN;

	while (rw.isOpen()) {
		if (state == STOP)
			break;
		sf::Event e{};
		sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(rw);
		pos.y = sizeScaled.y - pos.y;
		while (rw.pollEvent(e)) {
			switch (e.type) {
				case sf::Event::Closed : {
					if (state == BLOCKED)break;
					manager->onExit();
					break;
				}
				case sf::Event::Resized : {
					sizeScaled = sf::Vector2f((float)e.size.width, (float)e.size.height);
					rw.setView(sf::View(sf::FloatRect({}, sizeScaled)));
					refresh(false);
					break;
				}
				case sf::Event::MouseButtonPressed : {
					if (state == BLOCKED)break;
					int res = scenes[currentScene]->onMouseEvent(mouseEvent::pressing, pos);
					if (res >= -1) {
						pressed = true;
					}
					break;
				}
				case sf::Event::MouseButtonReleased : {
					if (state == BLOCKED)break;
					int res = scenes[currentScene]->onMouseEvent(mouseEvent::release, pos);
					pressed = false;
					if (res > -1) {
						state = BLOCKED;
						manager->onBtnClick(res, pos);
					}
					break;
				}
				case sf::Event::TextEntered : {
					if (state == BLOCKED)break;
					manager->onKeyEvent(e.text.unicode);
					break;
				}
				default: //printf("strange event: %d\n", (int)e.type);
					break;
			}
		}
		if (pressed && state != BLOCKED) {
			scenes[currentScene]->onMouseEvent(mouseEvent::holding, pos);
		}
		refresh(true);
	}
	rw.close();
}
void window::startRenderCycle() {
	renderThread = std::thread(&window::renderCycle, this);
}

window::window(const std::string& name, sf::Vector2f size, uint32_t sceneCount) {
	scenes = std::vector<uiElement*>(sceneCount, nullptr);
	sizeScaled = sizeOrigin = size;
	this->name = name;
	state = PREPARE;
}
window::~window() {
	for(auto* s : scenes) {
		delete s;
	}
}
void window::wait() {
	renderThread.join();
}