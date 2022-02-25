#include "window.h"

void window::drawAll() {
	rw.clear();
	for (auto p : parts) {
		p->draw(this);
	}
	rw.display();
}
void window::drawText() { }
void window::drawImage(std::string src, box2 box, sf::Vector2u gridSize, uint32_t id) {
	box = {box.bottomLeft.x, sizeScaled.y - box.topRight.y, box.topRight.x, sizeScaled.y - box.bottomLeft.y};
	sf::Texture tex;
	std::string key = src + "|" + std::to_string(id) + "|" + std::to_string(gridSize.x) + "|" + std::to_string(gridSize.y);
	auto found = loadedTextures.find(key);
	if (found == loadedTextures.end()) {
		sf::Image img;
		img.loadFromFile(src);

		sf::Vector2u s = img.getSize();
		s = {s.x / gridSize.x, s.y / gridSize.y};
		sf::IntRect size = sf::IntRect(s.x * (id % gridSize.x), s.y * (id / gridSize.x), s.x, s.y);

		tex.loadFromImage(img, size);
		loadedTextures.insert(std::make_pair(key, tex));
	} else {
		tex = found->second;
	}
	sf::Sprite spr(tex);
	auto s = tex.getSize();
	spr.setScale((box.topRight - box.bottomLeft) / sf::Vector2f(s.x, s.y));
	spr.setPosition(box.bottomLeft);
	rw.draw(spr);
}

window::window(const std::string& name, sf::Vector2f size) {
	sizeScaled = sizeOrigin = size;
	rw.create(sf::VideoMode(size.x, size.y), name);
	loadedTextures = {};
}
void window::addUIPart(uiElement* uielptr) {
	parts.push_back(uielptr);
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
