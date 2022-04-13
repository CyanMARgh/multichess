#include "uielements.h"

namespace ui {
	int InvisibleButton::OnMouseEvent(MouseEvent event, sf::Vector2f pos) {
		int res = NO_HIT;
		if (IsClickable()) {
			switch (event) {
				case MouseEvent::pressing: {
					if (!IsInside(pos))break;
					SetPressed(true);
					SetFresh(false);
					res = HIT_NO_ACTION;
					break;
				}
				case MouseEvent::holding: break;
				case MouseEvent::release: {
					if (IsInside(pos)) res = (int)id;
					SetPressed(false);
					SetFresh(false);
					break;
				}
			}
		}
		return res;
	}
	InvisibleButton::InvisibleButton(Box2 zone, ScaleMode sm, uint32_t id) :Element(zone, sm), id(id) {
		SetClickable(true);
	}

	void Button::Draw(Window* w) {
		if (IsVisible()) {
			(IsPressed() ? sprp : sprf).Draw(w, boxScaled);
		}
	}
	Button::Button(Box2 zone, ScaleMode sm, const SpriteParam& parFree, const SpriteParam& parPressed, uint32_t id) :
			InvisibleButton(zone, sm, id), sprf(parFree), sprp(parPressed) { }
}