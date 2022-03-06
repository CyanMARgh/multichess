#include "uielements.h"

#include <utility>

sprite::sprite(const std::string& src, sf::Vector2u gridSize, uint32_t id) {
	sf::Image img;
	img.loadFromFile(src);
	sf::Vector2u s = img.getSize();
	s0 = {s.x / gridSize.x, s.y / gridSize.y};
	sf::IntRect rect((int)(s0.x * (id % gridSize.x)), (int)(s0.y * (id / gridSize.x)), (int)s0.x, (int)s0.y);
	tex.loadFromImage(img, rect);
	spr.setTexture(tex);
}
void sprite::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	spr.setPosition(zone.bottomLeft);
	sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f((float)s0.x, (float)s0.y);
	spr.setScale(s);
	w->rw.draw(spr);
}

uiElement::uiElement(box2 zone, scaleMode sm) :boxOrigin(zone), boxScaled(zone), sm(sm) { }
void uiElement::reshape(box2 parentBoxOrigin, box2 parentBoxScaled) {
	switch (sm) {
		case scaleMode::bindBL : {
			sf::Vector2f delta = parentBoxScaled.bottomLeft - parentBoxOrigin.bottomLeft;
			boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
			break;
		}
		case scaleMode::bindTR : {
			sf::Vector2f delta = parentBoxScaled.topRight - parentBoxOrigin.topRight;
			boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
			break;
		}
		case scaleMode::bindBR : {
			sf::Vector2f delta = parentBoxScaled.bottomRight() - parentBoxOrigin.bottomRight();
			boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
			break;
		}
		case scaleMode::bindTL : {
			sf::Vector2f delta = parentBoxScaled.topLeft() - parentBoxOrigin.topLeft();
			boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
			break;
		}
		case scaleMode::scaleXY: {
			boxScaled = parentBoxScaled / parentBoxOrigin * boxOrigin;
			break;
		}
		case scaleMode::fitCenter: {
			float sx = parentBoxScaled.width() / boxOrigin.width();
			float sy = parentBoxScaled.height() / boxOrigin.height();
			float s = sx > sy ? sy : sx;
			sf::Vector2f pbsr = parentBoxScaled.rad();
			boxScaled.topRight = boxOrigin.center() * s + pbsr;
			boxScaled.bottomLeft = -boxOrigin.rad() * s + pbsr;
			break;
		}
		case scaleMode::fullZone: {
			boxScaled = parentBoxScaled;
			break;
		}
	}
}
void uiElement::reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled) {
	reshape({{0, 0}, parentSizeOrigin}, {{0, 0}, parentBoxScaled});
}
bool uiElement::onMouseEvent(sf::Vector2f pos, mouseEvent event) {
	return false;
}
bool uiElement::isInside(sf::Vector2f pos) const {
	return boxScaled.isInside(pos);
}

box2 uiGroup::getSubBox(uint i) {
	return boxScaled;
}
void uiGroup::draw(window* w) {
	if (visible) {
		for (uiElement* e : parts) {
			if (e) e->draw(w);
		}
	}
}
bool uiGroup::onMouseEvent(sf::Vector2f pos, mouseEvent event) {
	if (event == mouseEvent::pressing) {
		if (boxScaled.isInside(pos)) {
			for (uint i = 0, n = parts.size(); i < n; i++) {
				if (parts[i] && parts[i]->onMouseEvent(pos, event)) {
					pressedPart = (int)i;
					return true;
				}
			}
		}
	} else {
		if (pressedPart > -1 && parts[pressedPart]) parts[pressedPart]->onMouseEvent(pos, event);
		if (event == mouseEvent::release) pressedPart = -1;
	}
	return false;
}
void uiGroup::reshape(box2 parentBoxOrigin, box2 parentBoxScaled) {
	uiElement::reshape(parentBoxOrigin, parentBoxScaled);
	for (uint i = 0, n = parts.size(); i < n; i++) {
		if (parts[i]) {
			parts[i]->reshape(boxOrigin, getSubBox(i));
		}
	}
}
uiGroup::uiGroup(box2 zone, scaleMode sm, uint count) :uiElement(zone, sm) {
	parts.resize(count, nullptr);
}
void uiGroup::addUIPart(uiElement& uiel) {
	parts.push_back(&uiel);
}
void uiGroup::setUIPart(uint i, uiElement& uiel) {
	if (i >= parts.size()) {
		parts.resize(i + 1, nullptr);
	}
	parts[i] = &uiel;
}

uiImage::uiImage(box2 zone, scaleMode sm, const std::string& src) :uiElement(zone, sm), spr(src) { }
void uiImage::draw(window* w) {
	if (visible) {
		spr.draw(w, boxScaled);
	}
}

uiTilemap::uiTilemap(box2 zone, scaleMode sm, const std::string& src, sf::Vector2u srcGridSize) :uiElement(zone, sm) {
	map = nullptr;
	gridSize = {}, this->srcGridSize = srcGridSize;
	sprites = new sprite* [srcGridSize.x * srcGridSize.y];
	for (int i = 0; i < srcGridSize.x * srcGridSize.y; i++) {
		sprites[i] = new sprite(src, srcGridSize, i);
	}
}
void uiTilemap::draw(window* w) {
	if (visible) {
		sf::Vector2u i;
		int j = 0;
		for (i.y = 0; i.y < gridSize.y; i.y++) {
			for (i.x = 0; i.x < gridSize.x; i.x++, j++) {
				auto box = boxScaled * box2(sf::Vector2f(i) / sf::Vector2f(gridSize), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(gridSize));
				sprites[map[j]]->draw(w, box);
			}
		}
	}
}
uiTilemap::~uiTilemap() {
	for (int i = 0; i < srcGridSize.x * srcGridSize.y; i++) {
		delete sprites[i];
	}
	delete[] map;
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

void uiButton::draw(window* w) {
	if (visible) {
		(isPressed ? sprp : sprf).draw(w, boxScaled);
	}
}
bool uiButton::onMouseEvent(sf::Vector2f pos, mouseEvent event) {
	switch (event) {
		case mouseEvent::pressing: {
			if (boxScaled.isInside(pos)) {
				isPressed = true;
				return true;
			}
			break;
		}
		case mouseEvent::release : {
			if (isPressed && boxScaled.isInside(pos)) {
				action();
			}
			isPressed = false;
			break;
		}
		case mouseEvent::holding:break;
	}
	return false;
}
uiButton::uiButton(box2 zone, scaleMode sm, const std::string& srcFree, const std::string& srcPressed, std::function<void()> action) :
		uiElement(zone, sm), sprf(srcFree), sprp(srcPressed), action(std::move(action)) {
	isPressed = false;
}

uiScene::uiScene(bool isActive, box2 zone) :uiGroup(zone, scaleMode::fullZone) {
	visible = isActive;
}
void uiScene::changeToNewScene(uiScene* other) {
	visible = false;
	other->visible = true;
}

box2 uiSideCut::getSubBox(uint i) {
	sf::Vector2f C = boxScaled.center();
	sf::Vector2f s = (boxScaled.topRight - boxScaled.bottomLeft) / subSize;
	float l = boxScaled.left();
	float r = boxScaled.right();
	float b = boxScaled.bottom();
	float t = boxScaled.top();
	if (s.x > s.y) {
		float cl = C.x - s.y * subSize.x / 2;
		float cr = C.x + s.y * subSize.x / 2;
		if (i == 0) {
			return {cl, b, cr, t};
		} else if (i == 1) {
			return {l, b, cl, t};
		} else {
			return {cr, b, r, t};
		}
	} else {
		float cb = C.y - s.x * subSize.y / 2;
		float ct = C.y + s.x * subSize.y / 2;
		if (i == 0) {
			return {l, cb, r, ct};
		} else if (i == 1) {
			return {l, b, r, cb};
		} else {
			return {l, ct, r, t};
		}
	}
}
uiSideCut::uiSideCut(sf::Vector2f subSize, box2 zone, scaleMode sm) :uiGroup(zone, sm, 3) {
	this->subSize = subSize;
}
