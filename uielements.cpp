#include "uielements.h"

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
int uiElement::onMouseEvent(mouseEvent event, sf::Vector2f pos) {
	return NO_HIT;
}
bool uiElement::isInside(sf::Vector2f pos) const {
	return boxScaled.isInside(pos);
}

bool uiElement::isVisible() const { return (flags >> 0) & 1; }
bool uiElement::isClickable() const { return (flags >> 1) & 1; }
bool uiElement::isPressed() const { return (flags >> 2) & 1; }
bool uiElement::isFresh() const { return (flags >> 3) & 1; }

void uiElement::setVisible(bool visible) { flags = (flags & ~(1 << 0)) | (visible << 0); }
void uiElement::setClickable(bool clickable) { flags = (flags & ~(1 << 1)) | (clickable << 1); }
void uiElement::setPressed(bool pressed) { flags = (flags & ~(1 << 2)) | (pressed << 2); }
void uiElement::setFresh(bool fresh) { flags = (flags & ~(1 << 3)) | (fresh << 3); }

void uiElement::draw(window* w) { }
sf::Vector2f uiElement::toUnit(sf::Vector2f pos) const {
	return boxScaled.inv() * pos;
}
void uiElement::addPartsOrdered(std::vector<uiElement*>& ordered) {
	ordered.push_back(this);
}

box2 uiGroup::getSubBox(uint i) {
	return boxScaled;
}
void uiGroup::draw(window* w) {
	if (isVisible()) {
		for (auto& e : parts) {
			if (e) e->draw(w);
		}
	}
}
int uiGroup::onMouseEvent(mouseEvent event, sf::Vector2f pos) {
	int res = NO_HIT;
	if (isClickable()) {
		switch (event) {
			case mouseEvent::pressing: {
				if (!isInside(pos))break;
				res = HIT_NO_ACTION;
				for (uint i = 0, n = parts.size(); i < n; i++) {
					if (!parts[i] || !parts[i]->isClickable()) continue;
					int sres = parts[i]->onMouseEvent(event, pos);
					if (sres != NO_HIT) {
						pressedPart = (int)i;
						res = sres;
						break;
					}
				}
				setPressed(true);
				break;
			}
			case mouseEvent::holding: {
				if (pressedPart >= 0) {
					parts[pressedPart]->onMouseEvent(event, pos);
				}
				break;
			}
			case mouseEvent::release: {
				if (pressedPart >= 0 && parts[pressedPart]) {
					res = parts[pressedPart]->onMouseEvent(event, pos);
				}
				setPressed(false);
				break;
			}
		}
	}
	return res;
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
	parts = std::vector<std::unique_ptr<uiElement>>(count);
	setClickable(true);
}
void uiGroup::addUIPart(uiElement* uiel) {
	parts.push_back(std::unique_ptr<uiElement>(uiel));
}
void uiGroup::setPart(uiElement* uiel, uint32_t id) {
	parts[id] = std::unique_ptr<uiElement>(uiel);
}
void uiGroup::addPartsOrdered(std::vector<uiElement*>& ordered) {
	for (const auto& p : parts) {
		p->addPartsOrdered(ordered);
	}
}

uiImage::uiImage(box2 zone, scaleMode sm, const spriteparam& src) :uiElement(zone, sm), spr(src) { }
void uiImage::draw(window* w) {
	if (isVisible()) {
		spr.draw(w, boxScaled);
	}
}

void uiText::draw(window* w) {
	if (isVisible()) {
		spr.draw(w, boxScaled);
	}
}
uiText::uiText(box2 zone, scaleMode sm, const std::string& textSrc, const std::string& fontSrc) :uiElement(zone, sm), spr(fontSrc, textSrc) { }
void uiText::setString(const std::string& s) {
	spr.setText(s);
}

void uiShader::draw(window* w) {
	if (isVisible()) {
		spr.draw(w, boxScaled);
	}
}
uiShader::uiShader(box2 zone, scaleMode sm, const std::string& src) :uiElement(zone, sm), spr(src) { }
void uiShader::setTime(float time) {
	spr.setTime(time);
	setFresh(false);
}

uiTilemap::uiTilemap(box2 zone, scaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg, uint32_t end) :uiElement(zone, sm) {
	map = {};
	gridSize = {};
	if (end == -1) end = srcGridSize.x * srcGridSize.y;
	tilemap.assign(end - beg, {});
	for (uint32_t i = 0, j = beg; j < end; i++, j++) {
		tilemap[i] = sprite(src, srcGridSize, j);
	}
}
box2 uiTilemap::subBox(sf::Vector2i i) {
	return boxScaled * box2(sf::Vector2f(i) / sf::Vector2f(gridSize), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(gridSize));
}
void uiTilemap::draw(window* w) {
	if (isVisible()) {
		sf::Vector2i i;
		int j = 0;
		for (i.y = 0; i.y < gridSize.y; i.y++) {
			for (i.x = 0; i.x < gridSize.x; i.x++, j++) {
				tilemap[map[j]].draw(w, subBox(i));
			}
		}
	}
}
void uiTilemap::setIndexes(std::vector<uint32_t> map_, sf::Vector2u gridSize_) {
	gridSize = gridSize_;
	if (gridSize.x * gridSize.y != map_.size())exit(-1);
	map = std::move(map_);
	setFresh(false);
}
void uiTilemap::setByIndex(uint32_t cellId, uint32_t texId) {
	map[cellId] = texId;
	setFresh(false);
}
sf::Vector2i uiTilemap::proj(sf::Vector2f pos) {
	pos = toUnit(pos);
	return {(int)(pos.x * (float)gridSize.x), (int)(pos.y * (float)gridSize.y)};
}

size_t uiTilemap::size() const {
	return gridSize.x * gridSize.y;
}

int uiInvisibleButton::onMouseEvent(mouseEvent event, sf::Vector2f pos) {
	int res = NO_HIT;
	if (isClickable()) {
		switch (event) {
			case mouseEvent::pressing: {
				if (!isInside(pos))break;
				setPressed(true);
				setFresh(false);
				res = HIT_NO_ACTION;
				break;
			}
			case mouseEvent::holding: break;
			case mouseEvent::release: {
				if (isInside(pos)) res = (int)id;
				setPressed(false);
				setFresh(false);
				break;
			}
		}
	}
	return res;
}
uiInvisibleButton::uiInvisibleButton(box2 zone, scaleMode sm, uint32_t id) :uiElement(zone, sm), id(id) {
	setClickable(true);
}
void uiButton::draw(window* w) {
	if (isVisible()) {
		(isPressed() ? sprp : sprf).draw(w, boxScaled);
	}
}
uiButton::uiButton(box2 zone, scaleMode sm, const spriteparam& parFree, const spriteparam& parPressed, uint32_t id) :
		uiInvisibleButton(zone, sm, id), sprf(parFree), sprp(parPressed) { }

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

void uiSelectionTM::draw(window* w) {
	if (selPos != sf::Vector2i(-1, -1)) {
		spr.draw(w, tm->subBox(selPos));
	}
}
uiSelectionTM::uiSelectionTM(uiTilemap* tm, const spriteparam& src) :uiElement(tm->boxOrigin, tm->sm), spr(src) {
	this->tm = tm;
	selPos = {-1, -1};
}

sf::Vector2i uiSelectionTM::getSelPos() const {
	return selPos;
}
void uiSelectionTM::select(sf::Vector2i pos) {
	selPos = pos;
	setFresh(false);
}
void uiSelectionTM::click(sf::Vector2i pos) {
	selPos = (selPos == pos) ? sf::Vector2i(-1, -1) : pos;
	setFresh(false);
}

//selector::selector(uint count) {
//	variants.resize(count, nullptr);
//	selected = -1;
//}
//void selector::edit(uint id) {
//	if (selected == id) {
//		variants[id]->setSelection(false);
//		selected = -1;
//	} else {
//		if (selected != -1)variants[selected]->setSelection(false);
//		variants[id]->setSelection(true);
//		selected = (int)id;
//	}
//}
//int selector::getSelected() const {
//	return selected;
//}
//void selector::setVariant(uiSelectable& el, uint id) {
//	variants[id] = &el;
//}
//
//void uiSelectable::draw(window* w) {
//	uiButton::draw(w);
//	if (isSelected) {
//		selSprite->draw(w, boxScaled);
//	}
//}
//uiSelectable::uiSelectable(box2 zone, scaleMode sm, sprite* selSprite, selector& sel, uint id, const spriteparam& srcFree, const spriteparam& srcPressed) :
//		uiButton(zone, sm, srcFree, srcPressed, [id, &sel]() {
//			sel.edit(id);
//		}), selSprite(selSprite) {
//	isSelected = false;
//	sel.setVariant(*this, id);
//}
//void uiSelectable::setSelection(bool mode) {
//	isSelected = mode;
//}
