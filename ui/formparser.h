#pragma once

#include <fstream>
#include "uielements.h"
#include <map>
#include <sstream>

class Parser {
	std::map<std::string, ui::Element*> elements{};
	std::vector<ui::Element*> scenes{};
public:
	static ui::ScaleMode ParseScaleMode(std::ifstream& fin);
	static std::string ParseString(std::ifstream& fin);
	static Box2 ParseBox(std::ifstream& fin);
	static uint32_t ParseHexColor(std::ifstream& fin);
	static ui::Sprite::Param ParseSpriteParam(std::ifstream& fin);
	static void ToLower (std::string& s);

	static std::string FullName(std::vector<std::string>& groupStack, const std::string& name);
	std::array<ui::Element*, 2> ParseElement(std::vector<std::string>& groupStack, std::ifstream& fin);
	void Parse(const std::string& path, ui::Window& w, uint32_t scene0 = 0);

	ui::Element* operator[](const std::string&);

	template<class T>
	T* get(const std::string& s) {
		return operator[](s);
	}
};