#include <cstdio>
#include <SFML/Graphics.hpp>
#include <vector>
#include "box2.h"

//enum class scaleMode {
//	bindTopLeft, stretchXY
//};
//
//class window;
//
//class uielement {
//protected:
//	scaleMode sm;
//	box2 boxOrigin;
//	box2 boxScaled;
//
//	void reshape(box2 newBox) {
//		boxScaled = newBox;
//	}
//	void reshape(box2 parentBoxOrigin, box2 parentBoxScaled) {
//		if (sm == scaleMode::bindTopLeft) {
//			sf::Vector2f delta = parentBoxScaled.bottomLeft - parentBoxOrigin.bottomLeft;
//			boxScaled = {boxOrigin.bottomLeft + delta, boxOrigin.topRight + delta};
//		} else {
//			boxScaled = parentBoxScaled / parentBoxOrigin * boxOrigin;
//		}
//	}
//
//	virtual void draw(window*) = 0;
//	friend class window;
//};
//
//class window {
//	std::vector<uielement*> parts;
//	sf::RenderWindow rw;
//
//	void drawText() { }
//	void drawImage(std::string, box2) { }
//
//	void drawAll() {
//		for (auto p : parts) {
//			p->draw(this);
//		}
//	}
//public:
//	window(const std::string& name, sf::Vector2f size) {
//		rw.create(sf::VideoMode(size.x, size.y), name);
//	}
//	void addUIPart(uielement* uielptr) {
//		parts.push_back(uielptr);
//	}
//	void startRenderCycle() {
//
//	}
//};

int main() {
	box2 A(0, 0, 2, 3);
	box2 B(0, 0, 1, 2);

	(A * B).print();

	return 0;
}