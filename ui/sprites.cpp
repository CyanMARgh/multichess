#include "uielements.h"

namespace ui {
	std::unordered_set<std::shared_ptr<Sprite::Base>> Sprite::loadedSprites = {};
	Sprite::Base::Base(const sf::Image& img, const std::string& name, sf::IntRect rect) {
		tex.loadFromImage(img, rect);
		spr.setTexture(tex);
		this->name = name;
		s0 = sf::Vector2u(rect.width, rect.height);
	}
	void Sprite::Base::Draw(Window* w, Box2 zone) {
		auto sizeScaled = w->sizeScaled;
		zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
		spr.setPosition(zone.bottomLeft);
		sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f((float)s0.x, (float)s0.y);
		spr.setScale(s);
		w->rw.draw(spr);
	}
	void Sprite::LoadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics) {
		std::string prefix = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")";
		auto _s = std::find_if(loadedSprites.begin(), loadedSprites.end(), [&prefix](const auto& item) {
			return item->name == prefix + "0";
		});
		if (_s != loadedSprites.end()) return;

		sf::Image img;
		img.loadFromFile(src);

		sf::Vector2u s = img.getSize();
		sf::Vector2u s0 = {s.x / gridMetrics.x, s.y / gridMetrics.y};

		for (uint32_t y = 0, i = 0; y < gridMetrics.y; y++) {
			for (uint32_t x = 0; x < gridMetrics.x; x++, i++) {
				sf::IntRect rect((int)(s0.x * x), (int)(s0.y * y), (int)s0.x, (int)s0.y);
				std::shared_ptr<Base> sb(new Base(img, prefix + std::to_string(i), rect));
				loadedSprites.insert(std::move(sb));
			}
		}
	}
	Sprite::Param::Type Sprite::Param::GetType(const std::string& s) {
		if (s == "scale9") {
			return SCALE9;
		} else if (s == "none") {
			return NONE;
		} else if (s == "single" || s == "@") {
			return SINGLE;
		}
		assert(0);
	}

	Sprite::Sprite() {
		sbptrs = {};
		params = {""};
	}
	Sprite::Sprite(const Param& par) {
		params = par;
		uint32_t sscount =
				par.type == Param::SINGLE ? 1 :
				par.type == Param::SCALE9 ? 9 : 0;

		std::string srcf = par.s + "(" + std::to_string(par.gm.x) + ";" + std::to_string(par.gm.y) + ")";
		LoadSpriteSheet(par.s, par.gm);
		sbptrs = std::vector<std::shared_ptr<Base>>(sscount, nullptr);
		for (uint32_t i = 0; i < sscount; i++) {
			sbptrs[i] = *std::find_if(loadedSprites.begin(), loadedSprites.end(), [&](const auto& item) {
				return item->name == srcf + std::to_string(par.beg + i);
			});
		}
	}
	void Sprite::Draw(Window* w, Box2 zone) {
		switch (params.type) {
			case Param::NONE: {
				assert(false);
			}
			case Param::SINGLE: {
				sbptrs[0]->Draw(w, zone);
				break;
			}
			case Param::SCALE9: {
				float xs[4] = {zone.Left(), zone.Left() + params.size0.x * .5f, zone.Right() - params.size0.x * .5f, zone.Right()};
				float ys[4] = {zone.Bottom(), zone.Bottom() + params.size0.y * .5f, zone.Top() - params.size0.y * .5f, zone.Top()};
				for (int y = 2, i = 0; y >= 0; y--) {
					for (int x = 0; x < 3; x++, i++) {
						sbptrs[i]->Draw(w, {xs[x], ys[y], xs[x + 1], ys[y + 1]});
					}
				}
				break;
			}
		}
	}

	TextSprite::TextSprite(const std::string& fontSrc, const std::string& textSrc) {
		if (!font.loadFromFile(fontSrc)) {
			throw std::runtime_error("can't load font");
		}
		text.setFont(font);
		stext = textSrc;
	}
	void TextSprite::SetText(const std::string& textSrc) {
		stext = textSrc;
	}
	void TextSprite::Draw(Window* w, Box2 zone, uint32_t color) {
		auto sizeScaled = w->sizeScaled;
		zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
		text.setCharacterSize((uint)zone.Height());
		text.setString(stext);
		auto lb = text.getLocalBounds();
		uint32_t ns = std::count(stext.begin(), stext.end(), '\n') + 1;
		//printf(">%f\n", lb.Width / (stext.Size() - 1));
		text.setScale(sf::Vector2f(1, 1) * std::min((float)(.9 * zone.Width() / lb.width), .9f / (float)ns));

		lb = text.getGlobalBounds();
		sf::Vector2f c = ((Box2)lb).Rad();
		text.setPosition(zone.Center() - c);
		text.setFillColor(*(sf::Color*)&color);
		w->rw.draw(text);
	}

	ShaderSprite::ShaderSprite(const std::string& src) {
		sh.loadFromFile(src, sf::Shader::Fragment);
	}
	void ShaderSprite::Draw(Window* w, Box2 zone) {
//		printf("[4]\n");
		auto sizeScaled = w->sizeScaled;

		SetUniform("u_bl", (sf::Glsl::Vec2)zone.bottomLeft);
		SetUniform("u_tr", (sf::Glsl::Vec2)zone.topRight);

		for (const auto& p : uniformsFloat) {
			sh.setUniform(p.first, p.second);
		}
		for (const auto& p : uniformsVec2) {
			sh.setUniform(p.first, p.second);
		}
		for (const auto& p : uniformsTex) {
			sh.setUniform(p.first, *p.second);
		}

		zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
		rs.setSize(zone.Rad2());
		rs.setPosition(zone.bottomLeft);
		w->rw.draw(rs, &sh);
	}
}
