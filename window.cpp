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
	rw.create(sf::VideoMode(size.x, size.y), name);
}
void window::addUIPart(uiElement* uielptr) {
	parts.push_back(uielptr);
	uielptr->reshape(sizeOrigin, sizeScaled);
}
void window::startRenderCycle() {
	while (rw.isOpen()) {
		sf::Event e;
		while (rw.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				rw.close();
			} else if (e.type == sf::Event::Resized) {
				sf::Texture tex;
				tex.update(rw);
				//rw.clear();
				sizeScaled = sf::Vector2f(e.size.width, e.size.height);
				rw.setView(sf::View(sf::FloatRect({}, sizeScaled)));
				//rw.draw(sf::Sprite(tex));
				//rw.display();
				for (auto part : parts) {
					part->reshape(sizeOrigin, sizeScaled);
				}
			}
		}
		//sf::sleep(sf::milliseconds(100));
		drawAll();
	}
}
