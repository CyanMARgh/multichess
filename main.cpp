#include <cstdio>
#include "window.h"

int main() {
	window mainWindow("chess?", sf::Vector2f(800, 800));
	uiImage p0 = uiImage({0, 0, 400, 400}, scaleMode::bindBL, "src3.png");
	uiImage p1 = uiImage({400, 400, 800, 800}, scaleMode::bindTR, "src3.png");
	uiTilemap p2 = uiTilemap({0, 400, 400, 800}, scaleMode::bindTL, "src3.png", {2, 2});

	mainWindow.addUIPart(&p0);
	mainWindow.addUIPart(&p1);
	mainWindow.addUIPart(&p2);

	uint32_t* ids = new uint32_t[25]{0, 1, 2, 3, 0, 1, 2, 3, 0, 0, 1, 2, 3, 0, 1, 2, 3, 0, 0, 1, 2, 3, 0, 1, 2};
	p2.setIndexes(ids, sf::Vector2u(5, 5));
	delete[] ids;

	mainWindow.startRenderCycle();


	return 0;
}