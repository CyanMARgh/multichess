#include <cstdio>
#include "window.h"

int main() {
	window mainWindow("chess?", sf::Vector2f(800, 800));

	std::function<void()> f0 = []() { printf("f0\n"); };
	std::function<void()> f1 = []() { printf("f1\n"); };

	uiSideCut g0({1,1});

	uiButton b0({0, 0, 400, 400}, scaleMode::fullZone, "src0.png", "src1.png", f0);
	uiButton b1({400, 0, 800, 400}, scaleMode::fullZone, "src0.png", "src1.png", f1);


	g0.setUIPart(0, b0);
	g0.setUIPart(1, b1);

	mainWindow.setUIScene(g0);

	mainWindow.startRenderCycle();

	return 0;
}