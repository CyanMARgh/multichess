#include "window.h"

void window::drawAll() {
	rw.clear();
	for (auto p : parts) {
		p->draw(this);
	}
	rw.display();
}

window::window(const std::string& name, sf::Vector2f size) {
	sizeScaled = sizeOrigin = size;
	rw.create(sf::VideoMode((uint)size.x, (uint)size.y), name);
}
void window::addUIPart(uiElement& uiel) {
	parts.push_back(&uiel);
	uiel.reshape(sizeOrigin, sizeScaled);
}
void window::startRenderCycle() {
	//bool IsButtonPressed = false;
	uiElement* pressed = nullptr;

	while (rw.isOpen()) {
		sf::Event e{};
		while (rw.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				rw.close();
			} else if (e.type == sf::Event::Resized) {
				sizeScaled = sf::Vector2f((float)e.size.width, (float)e.size.height);
				rw.setView(sf::View(sf::FloatRect({}, sizeScaled)));
				for (auto part : parts) {
					part->reshape(sizeOrigin, sizeScaled);
				}
			} else if (e.type == sf::Event::MouseButtonPressed) {
				sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(rw);
				pos.y = sizeScaled.y - pos.y;
				for (auto el : parts) {
					if (el->onClick(pos, mouseEvent::pressing)) {
						pressed = el;
						break;
					}
				}
			} else if (e.type == sf::Event::MouseButtonReleased) {
				if(pressed) {
					sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(rw);
					pos.y = sizeScaled.y - pos.y;
					pressed->onClick(pos, mouseEvent::release);
					pressed = nullptr;
				}
			}
		}
		drawAll();
	}
}
