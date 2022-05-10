#include "uielements.h"

namespace ui {
	int InvisibleButton::OnMouseEvent(MouseEvent event, sf::Vector2f pos) {
		int res = NO_HIT;
		if (clickable) {
			switch (event) {
				case MouseEvent::pressing: {
					if (!IsInside(pos))break;
					pressed = true;
					fresh = false;
					res = HIT_NO_ACTION;
					break;
				}
				case MouseEvent::holding: break;
				case MouseEvent::release: {
					if (IsInside(pos)) res = (int)id;
					pressed = false;
					fresh = false;
					break;
				}
			}
		}
		return res;
	}
	InvisibleButton::InvisibleButton(Box2 zone, ScaleMode sm, uint32_t id) :Element(zone, sm), id(id) {
		clickable = true;
	}

	void Button::Draw(Window* w) {
		if (visible) {
			(pressed ? sprp : sprf).Draw(w, boxScaled);
		}
	}
	Button::Button(Box2 zone, ScaleMode sm, const Sprite::Param& parFree, const Sprite::Param& parPressed, uint32_t id) :
			InvisibleButton(zone, sm, id), sprf(parFree), sprp(parPressed) {
		visible = true;
	}
}