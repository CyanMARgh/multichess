#include "uielements.h"

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

uiImage::uiImage(box2 zone, scaleMode sm, std::string src) :uiElement(zone, sm) {
	this->src = src;
}
void uiImage::draw(window* w) {
	w->drawImage(src, boxScaled);
}

uiTilemap::uiTilemap(box2 zone, scaleMode sm, std::string src, sf::Vector2u count) :uiElement(zone, sm) {
	map = NULL;
	metrics = {}, this->count = count;
	this->src = src;
}
void uiTilemap::draw(window* w) {
	sf::Vector2u i;
	int j = 0;
	for (i.y = 0; i.y < metrics.y; i.y++) {
		for (i.x = 0; i.x < metrics.x; i.x++, j++) {
			w->drawImage(src,
					boxScaled * box2(sf::Vector2f(i) / sf::Vector2f(metrics), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(metrics)), count, map[j]);
		}
	}
}
void uiTilemap::setIndexes(const uint32_t* _map, sf::Vector2u _metrics) {
	size_t s = _metrics.x * _metrics.y;
	if (s != size()) {
		delete[] map;
		map = new uint32_t[s];
	}
	metrics = _metrics;
	memcpy(map, _map, s * sizeof(uint32_t));
}
size_t uiTilemap::size() const {
	return metrics.x * metrics.y;
}

