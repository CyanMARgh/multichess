#include "uielements.h"
#include <utility>

std::unordered_set<std::unique_ptr<sprite::base>> sprite::loadedSprites = {};
sprite::base::base(const sf::Image& img, const std::string& name, sf::IntRect rect) {
	tex.loadFromImage(img, rect);
	spr.setTexture(tex);
	this->name = name;
	s0 = sf::Vector2u(rect.width, rect.height);
}
void sprite::base::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	spr.setPosition(zone.bottomLeft);
	sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f((float)s0.x, (float)s0.y);
	spr.setScale(s);
	w->rw.draw(spr);
}
void sprite::loadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics) {
	//check if loaded

	sf::Image img;
	img.loadFromFile(src);

	std::string prefix = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")";

	sf::Vector2u s = img.getSize();
	sf::Vector2u s0 = {s.x / gridMetrics.x, s.y / gridMetrics.y};

	for (int y = 0, i = 0; y < gridMetrics.y; y++) {
		for (int x = 0; x < gridMetrics.x; x++, i++) {
			sf::IntRect rect((int)(s0.x * x), (int)(s0.y * y), (int)s0.x, (int)s0.y);
			std::unique_ptr<sprite::base> sb(new base(img, prefix + std::to_string(i), rect));
			loadedSprites.insert(std::move(sb));
		}
	}
}
sprite::sprite() {
	sbptr = nullptr;
}
sprite::sprite(const std::string& src, sf::Vector2u gridSize, uint32_t id) {
	std::string srcf = src + "(" + std::to_string(gridSize.x) + ";" + std::to_string(gridSize.y) + ")" + std::to_string(id);
	auto _s = std::find_if(loadedSprites.begin(), loadedSprites.end(), [&](const auto& item) {
		return item->name == srcf;
	});
	if (_s == loadedSprites.end()) {
		loadSpriteSheet(src, gridSize);
		sbptr = &**std::find_if(loadedSprites.begin(), loadedSprites.end(), [&](const auto& item) {
			return item->name == srcf;
		});
	} else {
		sbptr = &**_s;
	}
}
sprite::sprite(const spriteparam& par) : sprite(par.s, par.gm, par.i) { }
void sprite::draw(window* w, box2 zone) {
	if (sbptr)sbptr->draw(w, zone);
}

textSprite::textSprite(const std::string& fontSrc, const std::string& textSrc) {
	font.loadFromFile(fontSrc);
	text.setFont(font);
	stext = textSrc;
}
void textSprite::setText(const std::string& textSrc) {
	stext = textSrc;
}
void textSprite::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	text.setCharacterSize((uint)zone.height());
	text.setString(stext);
	auto lb = text.getLocalBounds();
	uint32_t ns = std::count(stext.begin(), stext.end(), '\n') + 1;
	text.setCharacterSize((uint)std::min(.8f * zone.height() * zone.width() / lb.width, zone.height() * .8f / (float)ns));
	// искуственный параметр
	lb = text.getLocalBounds();
	sf::Vector2f c = ((box2)lb).rad();
	text.setPosition(zone.center() - c);
	w->rw.draw(text);
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

uiImage::uiImage(box2 zone, scaleMode sm, const spriteparam& src) :uiElement(zone, sm), spr(src) { }
void uiImage::draw(window* w) {
	if (visible) {
		spr.draw(w, boxScaled);
	}
}

void uiText::draw(window* w) {
	if (visible) {
		spr.draw(w, boxScaled);
	}
}
uiText::uiText(box2 zone, scaleMode sm, const std::string& textSrc, const std::string& fontSrc) :uiElement(zone, sm), spr(fontSrc, textSrc) { }

uiTilemap::uiTilemap(box2 zone, scaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg, uint32_t end) :uiElement(zone, sm) {
	map = {};
	gridSize = {};
	if (end == -1) end = srcGridSize.x * srcGridSize.y;
	tilemap.reserve(end - beg);
	for (uint32_t i = 0, j = beg; i < srcGridSize.x * srcGridSize.y; i++, j++) {
		tilemap[i] = sprite(src, srcGridSize, j);
	}
}
void uiTilemap::draw(window* w) {
	if (visible) {
		sf::Vector2u i;
		int j = 0;
		for (i.y = 0; i.y < gridSize.y; i.y++) {
			for (i.x = 0; i.x < gridSize.x; i.x++, j++) {
				auto box = boxScaled * box2(sf::Vector2f(i) / sf::Vector2f(gridSize), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(gridSize));
				tilemap[map[j]].draw(w, box);
			}
		}
	}
}
void uiTilemap::setIndexes(std::vector<uint32_t> map_, sf::Vector2u gridSize_) {
	gridSize = gridSize_;
	if (gridSize.x * gridSize.y != map_.size())exit(-1);
	map = std::move(map_);
}
void uiTilemap::setByIndex(uint32_t cellId, uint32_t texId) {
	map[cellId] = texId;
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
				action(boxScaled.inv() * pos);
			}
			isPressed = false;
			break;
		}
		case mouseEvent::holding:break;
	}
	return false;
}
uiButton::uiButton(box2 zone, scaleMode sm, const spriteparam& parFree, const spriteparam& parPressed, std::function<void(sf::Vector2f)> action) :
		uiElement(zone, sm), action(std::move(action)), sprf(parFree), sprp(parPressed) {
	isPressed = false;
}
uiButton::uiButton(box2 zone, scaleMode sm, const spriteparam& parFree, const spriteparam& parPressed, const std::function<void()>& action) :
		uiButton(zone, sm, parFree, parPressed, [action](sf::Vector2f) { action(); }) {
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

box2 uiGrid::getSubBox(uint i) {
	sf::Vector2u I = {i % metrics.x, i / metrics.x};
	return boxScaled * box2(sf::Vector2f(I) / sf::Vector2f(metrics), (sf::Vector2f(I) + sf::Vector2f(1, 1)) / sf::Vector2f(metrics));
}
uiGrid::uiGrid(sf::Vector2u metrics, box2 zone, scaleMode sm) :uiGroup(zone, sm, metrics.x * metrics.y) {
	this->metrics = metrics;
}

selector::selector(uint count) {
	variants.resize(count, nullptr);
	selected = -1;
}
void selector::edit(uint id) {
	if (selected == id) {
		variants[id]->setSelection(false);
		selected = -1;
	} else {
		if (selected != -1)variants[selected]->setSelection(false);
		variants[id]->setSelection(true);
		selected = (int)id;
	}
}
int selector::getSelected() const {
	return selected;
}
void selector::setVariant(uiSelectable& el, uint id) {
	variants[id] = &el;
}

void uiSelectable::draw(window* w) {
	uiButton::draw(w);
	if (isSelected) {
		selSprite->draw(w, boxScaled);
	}
}
uiSelectable::uiSelectable(box2 zone, scaleMode sm, sprite* selSprite, selector& sel, uint id, const spriteparam& srcFree, const spriteparam& srcPressed) :
		uiButton(zone, sm, srcFree, srcPressed, [id, &sel]() {
			sel.edit(id);
		}), selSprite(selSprite) {
	isSelected = false;
	sel.setVariant(*this, id);
}
void uiSelectable::setSelection(bool mode) {
	isSelected = mode;
}
