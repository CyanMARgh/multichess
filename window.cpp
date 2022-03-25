#include "window.h"

sf::Vector2f window::getActualSize(sf::Vector2f s) const {
	return s;
}
void window::drawAll() {
	rw.clear();
	scene->draw(this);
	rw.display();
}
window::window(const std::string& name, sf::Vector2f size, float minSize, float minQ, bool hv) {
	this->minSize = minSize, this->minQ = minQ, this->hv = hv;
	sizeScaled = sizeOrigin = size;
	rw.create(sf::VideoMode((uint)size.x, (uint)size.y), name);
}
void window::setUIScene(uiElement& uiel) {
	scene = &uiel;
	uiel.reshape(sizeOrigin, sizeScaled);
}
void window::startRenderCycle() {
	if (scene == nullptr)throw std::invalid_argument("scene not set");
	bool pressed = false;

	while (rw.isOpen()) {
		sf::Event e{};
		sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(rw);
		pos.y = sizeScaled.y - pos.y;
		while (rw.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				rw.close();
			} else if (e.type == sf::Event::Resized) {
				sizeScaled = sf::Vector2f((float)e.size.width, (float)e.size.height);
				rw.setView(sf::View(sf::FloatRect({}, sizeScaled)));
				scene->reshape(sizeOrigin, sizeScaled);
			} else if (e.type == sf::Event::MouseButtonPressed) {
				scene->onMouseEvent(pos, mouseEvent::pressing);
				pressed = true;
			} else if (e.type == sf::Event::MouseButtonReleased) {
				scene->onMouseEvent(pos, mouseEvent::release);
				pressed = false;
			}
		}
		if(pressed) {
			scene->onMouseEvent(pos, mouseEvent::holding);
		}
		drawAll();
	}
}
