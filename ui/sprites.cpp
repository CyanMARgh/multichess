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
		sbptr = nullptr;
	}
	Sprite::Sprite(const std::string& src, sf::Vector2u gridMetrics, uint32_t id) {
		std::string srcf = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")" + std::to_string(id);
		LoadSpriteSheet(src, gridMetrics);
		sbptr = *std::find_if(SpriteBase::loadedSprites.begin(), SpriteBase::loadedSprites.end(), [&srcf](const auto& item) {
			return item->name == srcf;
		});
	}
	Sprite::Sprite(const SpriteParam& par) :Sprite(par.s, par.gm, par.i) { }
	void Sprite::Draw(Window* w, Box2 zone) {
		if (sbptr)sbptr->Draw(w, zone);
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
