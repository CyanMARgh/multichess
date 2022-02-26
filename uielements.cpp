#include "uielements.h"

sprite::sprite(std::string src, sf::Vector2u gridSize, uint32_t id) {
	sf::Image img;
	img.loadFromFile(src);
	sf::Vector2u s = img.getSize();
	s0 = {s.x / gridSize.x, s.y / gridSize.y};
	auto rect = sf::IntRect(s0.x * (id % gridSize.x), s0.y * (id / gridSize.x), s0.x, s0.y);
	tex.loadFromImage(img, rect);
	spr.setTexture(tex);
}
void sprite::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	spr.setPosition(zone.bottomLeft);
	sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f(s0.x, s0.y);
	spr.setScale(s);
	w->rw.draw(spr);
}

uiElement::uiElement(box2 zone, scaleMode sm) :boxOrigin(zone), boxScaled(zone), sm(sm) { }
void uiElement::reshape(box2 parentBoxOrigin, box2 parentBoxScaled) {
	if (sm == scaleMode::bindBL) {
		sf::Vector2f delta = parentBoxScaled.bottomLeft - parentBoxOrigin.bottomLeft;
		boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
	} else if (sm == scaleMode::bindTR) {
		sf::Vector2f delta = parentBoxScaled.topRight - parentBoxOrigin.topRight;
		boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
	} else if (sm == scaleMode::bindBR) {
		sf::Vector2f delta = parentBoxScaled.bottomRight() - parentBoxOrigin.bottomRight();
		boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
	} else if (sm == scaleMode::bindTL) {
		sf::Vector2f delta = parentBoxScaled.topLeft() - parentBoxOrigin.topLeft();
		boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
	} else if (sm == scaleMode::scaleXY) {
		boxScaled = parentBoxScaled / parentBoxOrigin * boxOrigin;
	}
}
void uiElement::reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled) {
	reshape({{0, 0}, parentSizeOrigin}, {{0, 0}, parentBoxScaled});
}

uiImage::uiImage(box2 zone, scaleMode sm, std::string src) :uiElement(zone, sm), spr(src) {
	this->src = src;
}
void uiImage::draw(window* w) {
	spr.draw(w, boxScaled);
}

uiTilemap::uiTilemap(box2 zone, scaleMode sm, std::string src, sf::Vector2u srcGridSize) :uiElement(zone, sm) {
	map = NULL;
	gridSize = {}, this->srcGridSize = srcGridSize;
	sprites = new sprite* [srcGridSize.x * srcGridSize.y];
	for (int i = 0; i < srcGridSize.x * srcGridSize.y; i++) {
		sprites[i] = new sprite(src, srcGridSize, i);
	}
	this->src = src;
}
void uiTilemap::draw(window* w) {
	sf::Vector2u i;
	int j = 0;
	for (i.y = 0; i.y < gridSize.y; i.y++) {
		for (i.x = 0; i.x < gridSize.x; i.x++, j++) {
			auto box = boxScaled * box2(sf::Vector2f(i) / sf::Vector2f(gridSize), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(gridSize));
			sprites[map[j]]->draw(w, box);
		}
	}
}
uiTilemap::~uiTilemap() {
	for (int i = 0; i < srcGridSize.x * srcGridSize.y; i++) {
		delete sprites[i];
	}
	delete[] sprites;
}
void uiTilemap::setIndexes(const uint32_t* _map, sf::Vector2u _gridSize) {
	size_t s = _gridSize.x * _gridSize.y;
	if (s != size()) {
		delete[] map;
		map = new uint32_t[s];
	}
	gridSize = _gridSize;
	memcpy(map, _map, s * sizeof(uint32_t));
}
size_t uiTilemap::size() const {
	return gridSize.x * gridSize.y;
}

