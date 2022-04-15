#include "uielements.h"

namespace ui {
	std::unordered_set<std::shared_ptr<SpriteBase>> SpriteBase::loadedSprites = {};
	SpriteBase::SpriteBase(const sf::Image& img, const std::string& name, sf::IntRect rect) {
		tex.loadFromImage(img, rect);
		spr.setTexture(tex);
		this->name = name;
		s0 = sf::Vector2u(rect.width, rect.height);
	}
	void SpriteBase::Draw(Window* w, Box2 zone) {
		auto sizeScaled = w->sizeScaled;
		zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
		spr.setPosition(zone.bottomLeft);
		sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f((float)s0.x, (float)s0.y);
		spr.setScale(s);
		w->rw.draw(spr);
	}
	void Sprite::LoadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics) {
		std::string prefix = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")";
		auto _s = std::find_if(SpriteBase::loadedSprites.begin(), SpriteBase::loadedSprites.end(), [&prefix](const auto& item) {
			return item->name == prefix + "0";
		});
		if (_s != SpriteBase::loadedSprites.end()) return;

		sf::Image img;
		img.loadFromFile(src);

		sf::Vector2u s = img.getSize();
		sf::Vector2u s0 = {s.x / gridMetrics.x, s.y / gridMetrics.y};

		for (int y = 0, i = 0; y < gridMetrics.y; y++) {
			for (int x = 0; x < gridMetrics.x; x++, i++) {
				sf::IntRect rect((int)(s0.x * x), (int)(s0.y * y), (int)s0.x, (int)s0.y);
				std::shared_ptr<SpriteBase> sb(new SpriteBase(img, prefix + std::to_string(i), rect));
				SpriteBase::loadedSprites.insert(std::move(sb));
			}
		}
	}
	Sprite::Sprite() {
		sbptrs = {};
		type = {};
	}
	Sprite::Sprite(const SpriteParam& par) {
		this->type = par.type;
		uint32_t sscount =
				type == SpriteParam::SINGLE ? 1 :
				type == SpriteParam::SCALE9 ? 9 : 0;

		std::string srcf = par.s + "(" + std::to_string(par.gm.x) + ";" + std::to_string(par.gm.y) + ")";
		LoadSpriteSheet(par.s, par.gm);
		sbptrs = std::vector<std::shared_ptr<SpriteBase>>(sscount, nullptr);
		for (uint32_t i = 0; i < sscount; i++) {
			sbptrs[i] = *std::find_if(SpriteBase::loadedSprites.begin(), SpriteBase::loadedSprites.end(), [&](const auto& item) {
				return item->name == srcf + std::to_string(par.i + i);
			});
		}
	}
	void Sprite::Draw(Window* w, Box2 zone) {
		switch (type) {
			case SpriteParam::NONE: {
				assert(false);
			}
			case SpriteParam::SINGLE: {
				sbptrs[0]->Draw(w, zone);
				break;
			}
			case SpriteParam::SCALE9: {
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
	void TextSprite::Draw(Window* w, Box2 zone) {
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
		w->rw.draw(text);
	}

	ShaderSprite::ShaderSprite(const std::string& src) {
		assert(sh.loadFromFile(src, sf::Shader::Fragment));
		time = 0;
	}
	void ShaderSprite::SetTime(float _time) {
		time = _time;
	}
	void ShaderSprite::Draw(Window* w, Box2 zone) {
		auto sizeScaled = w->sizeScaled;
		sh.setUniform("u_time", time);
		sh.setUniform("u_bl", (sf::Glsl::Vec2)zone.bottomLeft);
		sh.setUniform("u_tr", (sf::Glsl::Vec2)zone.topRight);

		zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
		rs.setSize(zone.Rad2());
		rs.setPosition(zone.bottomLeft);
		w->rw.draw(rs, &sh);
	}
}
