#include "window.h"

void appManager::onKeyEvent(uint32_t keyCode) { }
void appManager::onSceneSwitch(uint32_t sceneCode) { }
void appManager::onExit() {
	w->state = window::STOP;
}
void appManager::onBtnClick(uint32_t btnId, sf::Vector2f pos) { }
void appManager::switchScene(uint32_t sceneId) {
	currentScene = sceneId;
	w->switchScene(sceneId);
}
void appManager::unblock() {
	w->state = window::RUN;
}
appManager::appManager(window& w) :w(&w), currentScene(0) { }
void appManager::setUiPartsPtrs(std::vector<uiElement*> _uiels) {
	uiels = std::move(_uiels);
}

void window::refresh() {
	for (auto* s : scenes) {
		if (s->isVisible())s->reshape(sizeOrigin, sizeScaled);
	}
	rw.clear();
	for (auto* s : scenes) {
		s->draw(this);
	}
	rw.display();
}
void window::setScene(uiElement& uiel, uint32_t id) {
	scenes[id] = &uiel;
	uiel.reshape(sizeOrigin, sizeScaled);
}
void window::setManager(appManager& _manager) {
	manager = &_manager;
}
void window::switchScene(uint32_t sceneId) {
	scenes[currentScene]->visible = false;
	scenes[currentScene]->clickable = false;
	scenes[sceneId]->visible = true;
	scenes[sceneId]->clickable = true;
	currentScene = sceneId;
	refresh();
}

void window::renderCycle() {
	rw.create(sf::VideoMode((uint)sizeScaled.x, (uint)sizeScaled.y), name);
	for (int i = 0; i < scenes.size(); i++) {
		assert(scenes[i]);
		scenes[i]->visible = !i;
		scenes[i]->clickable = !i;
	}
	assert(manager);
	bool pressed = false;

	refresh();
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
					refresh();
					break;
				}
				case sf::Event::MouseButtonPressed : {
					if (state == BLOCKED)break;
					int res = scenes[currentScene]->onMouseEvent(mouseEvent::pressing, pos);
					if (res >= -1) {
						refresh();
						pressed = true;
					}
					break;
				}
				case sf::Event::MouseButtonReleased : {
					if (state == BLOCKED)break;
					int res = scenes[currentScene]->onMouseEvent(mouseEvent::release, pos);
					if (pressed)refresh();
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
void window::wait() {
	renderThread.join();
}