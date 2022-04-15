#include "formparser.h"

ui::Element* Parser::operator[](const std::string& name) {
	auto found = elements.find(name);
	assert(found != elements.end());
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
	char buf;
	do {
		buf = fin.get();
	} while (std::isspace(buf));
	if (buf != '"') assert(false);
	do {
		buf = fin.get();
		if (buf == '"')break;
		ss << buf;
	} while (true);
	return ss.str();
}
void Parser::ToLover(std::string& s) {
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
		assert(false);
	}
	return box;
}
ui::SpriteParam Parser::ParseSpriteParam(std::ifstream& fin) {
	std::string s = ParseString(fin), buf;
	fin >> buf;
	uint32_t sx, sy, i;
	if (buf == "@") {
		sx = sy = 1, i = 0;
	} else if (buf == ":") {
		fin >> sx >> sy >> i;
	} else {
		assert(false);
	}
	return {s, {sx, sy}, i};
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
std::vector<ui::Element*> Parser::ParseElement(std::vector<std::string>& groupStack, std::ifstream& fin) {
	std::string type, name;
	Box2 zone;
	fin >> type;
	ToLover(type);
	if (type == "@") return {};
	fin >> name;
	zone = ParseBox(fin);
	ui::ScaleMode sm = ParseScaleMode(fin);
	std::string fullname = FullName(groupStack, name);
	std::vector<ui::Element*> ans = {nullptr};
	if (type == "group") {
		ans[0] = new ui::Group(zone, sm);
		groupStack.push_back(name);
		do {
			auto subEls = ParseElement(groupStack, fin);
			if (!subEls.empty()) {
				for (auto el : subEls) {
					((ui::Group*)ans[0])->AddUIPart(el);
				}
			} else {
				break;
			}
		} while (true);
		groupStack.pop_back();
	} else if (type == "image") {
		ans[0] = new ui::Image(zone, sm, ParseSpriteParam(fin));
	} else if (type == "button") {
		auto srcf = ParseSpriteParam(fin);
		auto srcp = ParseSpriteParam(fin);
		uint32_t btncode;
		fin >> btncode;
		ans[0] = new ui::Button(zone, sm, srcf, srcp, btncode);
	} else if (type == "text") {
		ans[0] = new ui::Text(zone, sm, ParseString(fin), ParseString(fin));
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
			ans.push_back(sel);
		} else if (sub != "@") {
			assert(false);
		}
	} else {
		assert(false);
	}
	elements.insert({fullname, ans[0]});
	if (groupStack.empty()) {
		for (auto el : ans) {
			scenes.push_back(el);
		}
	}
	return ans;
}
void Parser::Parse(const std::string& path, ui::Window& w) {
	std::ifstream fin(path);
	std::vector<std::string> groupStack = {};
	while (!ParseElement(groupStack, fin).empty());
	for (uint i = 0, n = scenes.size(); i < n; i++) {
		w.SetScene(scenes[i], i);
	}
}
