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
	static ui::SpriteParam ParseSpriteParam(std::ifstream& fin);
	static void ToLover (std::string& s);

	static std::string FullName(std::vector<std::string>& groupStack, const std::string& name);
	std::vector<ui::Element*> ParseElement(std::vector<std::string>& groupStack, std::ifstream& fin);
	void Parse(const std::string& path, ui::Window& w);

	ui::Element* operator[](const std::string&);

	template<class T>
	T* get(const std::string& s) {
		return operator[](s);
	}
};