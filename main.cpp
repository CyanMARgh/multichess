#include <cstdio>
#include "window.h"

int main() {
	window mainWindow("chess?", sf::Vector2f(800, 800));

	uiSideCut p0 = uiSideCut({1, 1});
	uiImage p1 = uiImage({0, 0, 800, 800}, scaleMode::fullZone, "src0.png");
	uiImage p2 = uiImage({0, 0, 800, 800}, scaleMode::fullZone, "src1.png");

	p0.setUIPart(0, p1);
	p0.setUIPart(1, p2);

	mainWindow.addUIPart(p0);

	mainWindow.startRenderCycle();

	return 0;
}