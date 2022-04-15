#include "uielements.h"

namespace ui {
	Box2 Group::getSubBox(uint i) {
		return boxScaled;
	}
	void Group::AddUIPart(Element* uiel) {
		parts.push_back(std::unique_ptr<Element>(uiel));
	}
	void Group::SetPart(Element* uiel, uint32_t id) {
		parts[id] = std::unique_ptr<Element>(uiel);
	}

	void Group::Reshape(Box2 parentBoxOrigin, Box2 parentBoxScaled) {
		Element::Reshape(parentBoxOrigin, parentBoxScaled);
		for (uint i = 0, n = parts.size(); i < n; i++) {
			if (parts[i]) {
				parts[i]->Reshape(boxOrigin, getSubBox(i));
			}
		}
	}
	void Group::AddPartsOrdered(std::vector<Element*>& ordered) {
		for (const auto& p : parts) {
			p->AddPartsOrdered(ordered);
		}
	}

	int Group::OnMouseEvent(MouseEvent event, sf::Vector2f pos) {
		int res = NO_HIT;
		if (Is(CLICKABLE)) {
			switch (event) {
				case MouseEvent::pressing: {
					if (!IsInside(pos))break;
					res = HIT_NO_ACTION;
					for (uint i = 0, n = parts.size(); i < n; i++) {
						if (!parts[i] || !parts[i]->Is(CLICKABLE)) continue;
						int sres = parts[i]->OnMouseEvent(event, pos);
						if (sres != NO_HIT) {
							pressedPart = (int)i;
							res = sres;
							break;
						}
					}
					Set(PRESSED, true);
					break;
				}
				case MouseEvent::holding: {
					if (pressedPart >= 0) {
						parts[pressedPart]->OnMouseEvent(event, pos);
					}
					break;
				}
				case MouseEvent::release: {
					if (pressedPart >= 0 && parts[pressedPart]) {
						res = parts[pressedPart]->OnMouseEvent(event, pos);
					}
					Set(PRESSED, false);
					break;
				}
			}
		}
		return res;
	}
	void Group::Draw(Window* w) {
		if (Is(VISIBLE)) {
			for (auto& e : parts) {
				if (e) e->Draw(w);
			}
		}
	}
	Group::Group(Box2 zone, ScaleMode sm, uint count) :Element(zone, sm) {
		parts = std::vector<std::unique_ptr<Element>>(count);
		Set(CLICKABLE, true);
	}

	Box2 SideCut::getSubBox(uint i) {
		sf::Vector2f C = boxScaled.Center();
		sf::Vector2f s = (boxScaled.topRight - boxScaled.bottomLeft) / subSize;
		float l = boxScaled.Left();
		float r = boxScaled.Right();
		float b = boxScaled.Bottom();
		float t = boxScaled.Top();
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
	SideCut::SideCut(sf::Vector2f subSize, Box2 zone, ScaleMode sm) :Group(zone, sm, 3) {
		this->subSize = subSize;
	}

	Box2 Grid::getSubBox(uint i) {
		sf::Vector2u I = {i % metrics.x, i / metrics.x};
		return boxScaled * Box2(sf::Vector2f(I) / sf::Vector2f(metrics), (sf::Vector2f(I) + sf::Vector2f(1, 1)) / sf::Vector2f(metrics));
	}
	Grid::Grid(sf::Vector2u metrics, Box2 zone, ScaleMode sm) :Group(zone, sm, metrics.x * metrics.y) {
		this->metrics = metrics;
	}
}