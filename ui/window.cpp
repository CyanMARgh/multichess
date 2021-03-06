#include "uielements.h"

namespace ui {
	void Window::Refresh(bool check) {
		if (!check) goto NOT_FRESH;
		for (auto* e : ordered) {
			if (!e->fresh && e->visible) {
				goto NOT_FRESH;
			}
		}
//		printf("[0]\n");
		return;
	NOT_FRESH:
		//printf("[1]\n");
		for (auto* e : ordered) {
			e->fresh = true;
		}
		for (auto* s : scenes) {
			if (s->visible)s->Reshape(sizeOrigin, sizeScaled);
		}
		scenes[currentScene]->Reshape(sizeOrigin, sizeScaled);
		rw.clear();
		scenes[currentScene]->Draw(this);
		rw.display();
	}
	void Window::SetScene(Element* uiel, uint32_t id) {
		scenes[id] = uiel;
	}
	void Window::SetManager(AppManager& _manager) {
		this->manager = &_manager;
	}
	void Window::SwitchScene(uint32_t sceneId) {
		scenes[currentScene]->visible = false;
		scenes[currentScene]->clickable = false;
		scenes[sceneId]->visible = true;
		scenes[sceneId]->clickable = true;
		currentScene = sceneId;
		Refresh(false);
	}

	void Window::RefreshMousePos() {
		mousePos = (sf::Vector2f)sf::Mouse::getPosition(rw);
		mousePos.y = sizeScaled.y - mousePos.y;
	}
	sf::Vector2f Window::GetMousePos() const {
		return mousePos;
	}

	void Window::RenderCycle() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 10;

		rw.create(sf::VideoMode((uint)sizeScaled.x, (uint)sizeScaled.y), name, sf::Style::Default, settings);
		for (uint i = 0; i < scenes.size(); i++) {
			if (!scenes[i]) throw std::runtime_error("scene #" + std::to_string(i) + " not set");
			scenes[i]->visible = scenes[i]->clickable = !i;
			scenes[i]->AddPartsOrdered(ordered);
		}
		assert(manager);
		bool pressed = false;

		Refresh(false);
		state = RUN;

		while (rw.isOpen()) {
			if (state == STOP) {
				rw.close();
				break;
			}
			sf::Event e{};
			RefreshMousePos();
			while (rw.pollEvent(e)) {
				switch (e.type) {
					case sf::Event::Closed : {
						if (state == BLOCKED)break;
						manager->OnExit();
						break;
					}
					case sf::Event::Resized : {
						sizeScaled = sf::Vector2f((float)e.size.width, (float)e.size.height);
						rw.setView(sf::View(sf::FloatRect({}, sizeScaled)));
						Refresh(false);
						break;
					}
					case sf::Event::MouseButtonPressed : {
						if (state == BLOCKED)break;
						int res = scenes[currentScene]->OnMouseEvent(MouseEvent::pressing, mousePos);
						if (res >= -1) {
							pressed = true;
						}
						break;
					}
					case sf::Event::MouseButtonReleased : {
						if (state == BLOCKED)break;
						int res = scenes[currentScene]->OnMouseEvent(MouseEvent::release, mousePos);
						pressed = false;
						if (res > -1) {
							state = BLOCKED;
							manager->OnBtnClick(res, mousePos);
						}
						break;
					}
					case sf::Event::TextEntered : {
						if (state == BLOCKED)break;
						manager->OnKeyEvent(e.text.unicode);
						break;
					}
					default: //printf("strange event: %d\n", (int)e.type);
						break;
				}
			}
			if (pressed && state != BLOCKED) {
				scenes[currentScene]->OnMouseEvent(MouseEvent::holding, mousePos);
			}
//			printf("%f %f\n", mousePos.x, mousePos.y);
			Refresh(true);
		}
		rw.close();
	}
	void Window::StartRenderCycle() {
		renderThread = std::thread(&Window::RenderCycle, this);
	}

	Window::Window(const std::string& name, sf::Vector2f size, uint32_t sceneCount) {
		scenes = std::vector<Element*>(sceneCount, nullptr);
		sizeScaled = sizeOrigin = size;
		this->name = name;
		state = PREPARE;
	}
	Window::~Window() {
		renderThread.join();
		for (auto* s : scenes) {
			delete s;
		}
	}
}
