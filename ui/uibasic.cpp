#include "uielements.h"

namespace ui {
	bool Element::IsInside(sf::Vector2f pos) const {
		return boxScaled.IsInside(pos);
	}
	sf::Vector2f Element::ToUnit(sf::Vector2f pos) const {
		return boxScaled.Inv() * pos;
	}

	bool Element::IsVisible() const { return (flags >> 0) & 1; }
	bool Element::IsClickable() const { return (flags >> 1) & 1; }
	bool Element::IsPressed() const { return (flags >> 2) & 1; }
	bool Element::IsFresh() const { return (flags >> 3) & 1; }

	void Element::SetVisible(bool visible) { flags = (flags & ~(1 << 0)) | (visible << 0); }
	void Element::SetClickable(bool clickable) { flags = (flags & ~(1 << 1)) | (clickable << 1); }
	void Element::SetPressed(bool pressed) { flags = (flags & ~(1 << 2)) | (pressed << 2); }
	void Element::SetFresh(bool fresh) { flags = (flags & ~(1 << 3)) | (fresh << 3); }

	void Element::AddPartsOrdered(std::vector<Element*>& ordered) {
		ordered.push_back(this);
	}
	void Element::Reshape(Box2 parentBoxOrigin, Box2 parentBoxScaled) {
		switch (sm) {
			case ScaleMode::bindBL : {
				sf::Vector2f delta = parentBoxScaled.bottomLeft - parentBoxOrigin.bottomLeft;
				boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
				break;
			}
			case ScaleMode::bindTR : {
				sf::Vector2f delta = parentBoxScaled.topRight - parentBoxOrigin.topRight;
				boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
				break;
			}
			case ScaleMode::bindBR : {
				sf::Vector2f delta = parentBoxScaled.BottomRight() - parentBoxOrigin.BottomRight();
				boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
				break;
			}
			case ScaleMode::bindTL : {
				sf::Vector2f delta = parentBoxScaled.TopLeft() - parentBoxOrigin.TopLeft();
				boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
				break;
			}
			case ScaleMode::scaleXY: {
				boxScaled = parentBoxScaled / parentBoxOrigin * boxOrigin;
				break;
			}
			case ScaleMode::fitCenter: {
				float sx = parentBoxScaled.Width() / boxOrigin.Width();
				float sy = parentBoxScaled.Height() / boxOrigin.Height();
				float s = sx > sy ? sy : sx;
				sf::Vector2f pbsr = parentBoxScaled.Rad();
				boxScaled.topRight = boxOrigin.Center() * s + pbsr;
				boxScaled.bottomLeft = -boxOrigin.Rad() * s + pbsr;
				break;
			}
			case ScaleMode::fullZone: {
				boxScaled = parentBoxScaled;
				break;
			}
		}
	}
	void Element::Reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled) {
		Reshape({{0, 0}, parentSizeOrigin}, {{0, 0}, parentBoxScaled});
	}

	int Element::OnMouseEvent(MouseEvent event, sf::Vector2f pos) {
		return NO_HIT;
	}
	void Element::Draw(Window* w) { }
	Element::Element(Box2 zone, ScaleMode sm) :boxOrigin(zone), boxScaled(zone), sm(sm) { }



	void Image::Draw(Window* w) {
		if (IsVisible()) {
			spr.Draw(w, boxScaled);
		}
	}
	Image::Image(Box2 zone, ScaleMode sm, const SpriteParam& src) :Element(zone, sm), spr(src) { }

	void Text::SetString(const std::string& s) {
		spr.SetText(s);
	}
	void Text::Draw(Window* w) {
		if (IsVisible()) {
			spr.Draw(w, boxScaled);
		}
	}
	Text::Text(Box2 zone, ScaleMode sm, const std::string& textSrc, const std::string& fontSrc) :Element(zone, sm), spr(fontSrc, textSrc) { }

	void Shader::SetTime(float time) {
		spr.SetTime(time);
		SetFresh(false);
	}
	void Shader::Draw(Window* w) {
		if (IsVisible()) {
			spr.Draw(w, boxScaled);
		}
	}
	Shader::Shader(Box2 zone, ScaleMode sm, const std::string& src) :Element(zone, sm), spr(src) { }
}