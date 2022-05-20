#include "formparser.h"

ui::Element* Parser::operator[](const std::string& name) {
	auto found = elements.find(name);
	if (found == elements.end()) {
		throw std::runtime_error("no Element with name \"" + name + "\"");
	}
	return found->second;
}
std::string Parser::FullName(std::vector<std::string>& groupStack, const std::string& name) {
	std::stringstream ss;
	for (const auto& s : groupStack) {
		ss << s << "_";
	}
	ss << name;
	return ss.str();
}
std::string Parser::ParseString(std::ifstream& fin) {
	std::stringstream ss;
	int buf;
	do {
		buf = fin.get();
	} while (std::isspace(buf));
	if (buf != '"') throw std::runtime_error("expected string literal");
	do {
		buf = fin.get();
		if (buf == '"')break;
		ss << (char)buf;
	} while (true);
	return ss.str();
}
void Parser::ToLower(std::string& s) {
	for (auto& c : s) {
		if (c >= 'A' && c <= 'Z') {
			c += ('a' - 'A');
		}
	}
}
Box2 Parser::ParseBox(std::ifstream& fin) {
	std::string buf;
	fin >> buf;
	Box2 box;
	if (buf == "@") {
		box = Box2::Unit();
	} else if (buf == ":") {
		fin >> box.bottomLeft.x >> box.bottomLeft.y >> box.topRight.x >> box.topRight.y;
	} else {
		throw std::runtime_error("can't parse Box2.");
	}
	return box;
}
uint32_t Parser::ParseHexColor(std::ifstream& fin) {
	std::stringstream ss;
	int buf;
	do {
		buf = fin.get();
	} while (std::isspace(buf));
	if (buf == '@')return ~0;
	if (buf != '#')throw std::runtime_error("unexpected character (" + std::to_string((char)buf) + ") on color parse");
	int ans = 0;
	for (uint32_t i = 0; i < 8; i++) {
		buf = fin.get();
		buf = tolower(buf);
		ans <<= 4;
		ans |= ((buf <= '9' && buf >= '0') ? buf - '0' : buf - 'a' + 10);
	}
	return ans;
}

ui::Sprite::Param Parser::ParseSpriteParam(std::ifstream& fin) {
	std::string s = ParseString(fin), buf;
	fin >> buf;
	uint32_t sx, sy, i;
	sf::Vector2f size0 = {0, 0};
	ui::Sprite::Param::Type t;
	if (buf == "@") {
		sx = sy = 1, i = 0;
		t = ui::Sprite::Param::Type::SINGLE;
	} else if (buf == ":") {
		fin >> sx >> sy >> i >> buf;
		ToLower(buf);
		t = ui::Sprite::Param::GetType(buf);
		if (t == ui::Sprite::Param::Type::SCALE9) {
			fin >> size0.x >> size0.y;
		}
	} else {
		throw std::runtime_error("can't parse sprite parameter");
	}
	return {s, {sx, sy}, i, t, size0};
}
ui::ScaleMode Parser::ParseScaleMode(std::ifstream& fin) {
	std::string s;
	fin >> s;
	using namespace ui;
	return s == "bindBL" ? ScaleMode::bindBL :
		   s == "bindTR" ? ScaleMode::bindTR :
		   s == "bindTL" ? ScaleMode::bindTL :
		   s == "bindBR" ? ScaleMode::bindBR :
		   s == "scaleXY" ? ScaleMode::scaleXY :
		   s == "fitCenter" ? ScaleMode::fitCenter :
		   ScaleMode::fullZone;
}
std::array<ui::Element*, 2> Parser::ParseElement(std::vector<std::string>& groupStack, std::ifstream& fin) {
	std::array<ui::Element*, 2> ans = {nullptr, nullptr};
	std::string type, name;
	Box2 zone;
	fin >> type;
	ToLower(type);
	if (type == "@") return {};
	fin >> name;
	zone = ParseBox(fin);
	ui::ScaleMode sm = ParseScaleMode(fin);
	std::string fullname = FullName(groupStack, name);
	if (type == "group" || type == "variant") {
		if (type == "group") {
			ans[0] = new ui::Group(zone, sm);
		} else {
			ans[0] = new ui::Variant(zone, sm);
		}
		groupStack.push_back(name);
		do {
			auto subEls = ParseElement(groupStack, fin);
			if (!subEls[0])break;
			for (auto el : subEls) {
				if (!el)break;
				((ui::Group*)ans[0])->AddUIPart(el);
			}
		} while (true);
		groupStack.pop_back();

		if (type == "variant") {
			((ui::Variant*)ans[0])->InitVisibility();
		}
	} else if (type == "image") {
		ans[0] = new ui::Image(zone, sm, ParseSpriteParam(fin));
	} else if (type == "shader") {
		ans[0] = new ui::Shader(zone, sm, ParseString(fin));
	} else if (type == "button") {
		auto srcf = ParseSpriteParam(fin);
		auto srcp = ParseSpriteParam(fin);
		uint32_t btncode;
		fin >> btncode;
		ans[0] = new ui::Button(zone, sm, srcf, srcp, btncode);
	} else if (type == "text") {
		ans[0] = new ui::Text(zone, sm, ParseString(fin), ParseString(fin), ParseHexColor(fin));
	} else if (type == "invisiblebutton") {
		uint32_t btncode;
		fin >> btncode;
		ans[0] = new ui::InvisibleButton(zone, sm, btncode);
	} else if (type == "tilemap") {
		std::string src = ParseString(fin);
		uint32_t sx, sy, beg, end;
		fin >> sx >> sy >> beg >> end;
		ans[0] = new ui::TileMap(zone, sm, src, {sx, sy}, beg, end);
		std::string sub;
		fin >> sub;
		if (sub == "sel") {
			auto sel = new ui::SelectionTM((ui::TileMap*)ans[0], ParseSpriteParam(fin));
			elements.insert({fullname + "_sel", sel});
			ans[1] = sel;
		} else if (sub != "@") {
			throw std::runtime_error("invalid parameter of tilemap: \"" + sub + "\"");
		}
	} else {
		throw std::runtime_error("unknown Element type: \"" + name + "\"");
	}
	elements.insert({fullname, ans[0]});
	if (groupStack.empty()) {
		for (auto el : ans) {
			scenes.push_back(el);
		}
	}
	return ans;
}
void Parser::Parse(const std::string& path, ui::Window& w, uint32_t scene0) {
	std::ifstream fin(path);
	std::vector<std::string> groupStack = {};
	while (ParseElement(groupStack, fin)[0]);
	for (uint i = 0, n = scenes.size(); i < n; i++) {
		w.SetScene(scenes[scene0 + i], i);
	}
}
