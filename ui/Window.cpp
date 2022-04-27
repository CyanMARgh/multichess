#include "Window.h"

namespace ui {
	void Window::Refresh(bool check) {
		if (!check) goto NOT_FRESH;
		for (auto* e : ordered) {
			if (!e->Is(Element::FRESH) && e->Is(Element::VISIBLE)) goto NOT_FRESH;
		}
		return;
	NOT_FRESH:
		for (auto* e : ordered) {
			e->Set(Element::FRESH, true);
		}
		for (auto* s : scenes) {
			if (s->Is(Element::VISIBLE))s->Reshape(sizeOrigin, sizeScaled);
		}
		scenes[currentScene]->Reshape(sizeOrigin, sizeScaled);
		rw.clear();
		scenes[currentScene]->Draw(this);
		rw.display();
	}
	void Window::SetScene(Element* uiel, uint32_t id) {
		scenes[id] = uiel;
	}
	void Window::SetManager(AppManager& manager) {
		this->manager = &manager;
	}
	void Window::SwitchScene(uint32_t sceneId) {
		scenes[currentScene]->Set(Element::VISIBLE, false);
		scenes[currentScene]->Set(Element::CLICKABLE, false);
		scenes[sceneId]->Set(Element::VISIBLE, true);
		scenes[sceneId]->Set(Element::CLICKABLE, true);
		currentScene = sceneId;
		Refresh(false);
	}

	void Window::RenderCycle() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 10;

		rw.create(sf::VideoMode((uint)sizeScaled.x, (uint)sizeScaled.y), name, sf::Style::Default, settings);
		for (int i = 0; i < scenes.size(); i++) {
			assert(scenes[i]);
			scenes[i]->Set(Element::VISIBLE, !i);
			scenes[i]->Set(Element::CLICKABLE, !i);
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
			sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(rw);
			pos.y = sizeScaled.y - pos.y;
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
						int res = scenes[currentScene]->OnMouseEvent(MouseEvent::pressing, pos);
						if (res >= -1) {
							pressed = true;
						}
						break;
					}
					case sf::Event::MouseButtonReleased : {
						if (state == BLOCKED)break;
						int res = scenes[currentScene]->OnMouseEvent(MouseEvent::release, pos);
						pressed = false;
						if (res > -1) {
							state = BLOCKED;
							manager->OnBtnClick(res, pos);
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
				scenes[currentScene]->OnMouseEvent(MouseEvent::holding, pos);
			}
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
	void Window::Wait() {
		//renderThread.join();
	}
	Window::~Window() {
		renderThread.join();
		for (auto* s : scenes) {
			delete s;
		}
	}
}
