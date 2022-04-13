#include "uielements.h"

std::unordered_set<std::shared_ptr<sprbase>> sprbase::loadedSprites = {};
sprbase::sprbase(const sf::Image& img, const std::string& name, sf::IntRect rect) {
	tex.loadFromImage(img, rect);
	spr.setTexture(tex);
	this->name = name;
	s0 = sf::Vector2u(rect.width, rect.height);
}
void sprbase::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	spr.setPosition(zone.bottomLeft);
	sf::Vector2f s = (zone.topRight - zone.bottomLeft) / sf::Vector2f((float)s0.x, (float)s0.y);
	spr.setScale(s);
	w->rw.draw(spr);
}
void sprite::loadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics) {
	std::string prefix = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")";
	auto _s = std::find_if(sprbase::loadedSprites.begin(), sprbase::loadedSprites.end(), [&prefix](const auto& item) {
		return item->name == prefix + "0";
	});
	if (_s != sprbase::loadedSprites.end()) return;

	sf::Image img;
	img.loadFromFile(src);

	sf::Vector2u s = img.getSize();
	sf::Vector2u s0 = {s.x / gridMetrics.x, s.y / gridMetrics.y};

	for (int y = 0, i = 0; y < gridMetrics.y; y++) {
		for (int x = 0; x < gridMetrics.x; x++, i++) {
			sf::IntRect rect((int)(s0.x * x), (int)(s0.y * y), (int)s0.x, (int)s0.y);
			std::shared_ptr<sprbase> sb(new sprbase(img, prefix + std::to_string(i), rect));
			sprbase::loadedSprites.insert(std::move(sb));
		}
	}
}
sprite::sprite() {
	sbptr = nullptr;
}
sprite::sprite(const std::string& src, sf::Vector2u gridMetrics, uint32_t id) {
	std::string srcf = src + "(" + std::to_string(gridMetrics.x) + ";" + std::to_string(gridMetrics.y) + ")" + std::to_string(id);
	loadSpriteSheet(src, gridMetrics);
	sbptr = *std::find_if(sprbase::loadedSprites.begin(), sprbase::loadedSprites.end(), [&srcf](const auto& item) {
		return item->name == srcf;
	});
}
sprite::sprite(const spriteparam& par) :sprite(par.s, par.gm, par.i) { }
void sprite::draw(window* w, box2 zone) {
	if (sbptr)sbptr->draw(w, zone);
}

textSprite::textSprite(const std::string& fontSrc, const std::string& textSrc) {
	if (!font.loadFromFile(fontSrc)) {
		throw std::runtime_error("can't load font");
	}
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
	//printf(">%f\n", lb.width / (stext.size() - 1));
	text.setScale(sf::Vector2f(1, 1) * std::min((float)(.9 * zone.width() / lb.width), .9f / (float)ns));

	lb = text.getGlobalBounds();
	sf::Vector2f c = ((box2)lb).rad();
	text.setPosition(zone.center() - c);
	w->rw.draw(text);
}

shaderSprite::shaderSprite(const std::string& src) {
	assert(sh.loadFromFile(src, sf::Shader::Fragment));
	time = 0;
}
void shaderSprite::setTime(float _time) {
	time = _time;
}
void shaderSprite::draw(window* w, box2 zone) {
	auto sizeScaled = w->sizeScaled;
	sh.setUniform("u_time", time);
	sh.setUniform("u_bl", (sf::Glsl::Vec2)zone.bottomLeft);
	sh.setUniform("u_tr", (sf::Glsl::Vec2)zone.topRight);

	zone = {zone.bottomLeft.x, sizeScaled.y - zone.topRight.y, zone.topRight.x, sizeScaled.y - zone.bottomLeft.y};
	rs.setSize(zone.rad2());
	rs.setPosition(zone.bottomLeft);
	w->rw.draw(rs, &sh);
}