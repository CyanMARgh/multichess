#include <cstdio>
#include "window.h"
#include <variant>

int main() {
	window mainWindow("~~~", {1200, 800});

	uiSideCut subCut({1, 1});
	uiGroup g0({0,0,800,800}, scaleMode::fullZone, 2);

	uiGrid selGrid({1, 4}, {150, 200, 250, 600}, scaleMode::scaleXY);
	sprite selSprite({"src4.png"});
	selector sel(4);
	uiSelectable v0(box2::unit(), scaleMode::fullZone, &selSprite, sel, 0, {"src0.png"}, {"src1.png"});
	uiSelectable v1(box2::unit(), scaleMode::fullZone, &selSprite, sel, 1, {"src0.png"}, {"src1.png"});
	uiSelectable v2(box2::unit(), scaleMode::fullZone, &selSprite, sel, 2, {"src0.png"}, {"src1.png"});
	uiSelectable v3(box2::unit(), scaleMode::fullZone, &selSprite, sel, 3, {"src0.png"}, {"src1.png"});
	selGrid.setUIPart(0, v0);
	selGrid.setUIPart(1, v1);
	selGrid.setUIPart(2, v2);
	selGrid.setUIPart(3, v3);

	uiTilemap t0({300, 200, 700, 600}, scaleMode::scaleXY, "src3.png", {2, 2});
	t0.setIndexes(std::vector<uint32_t>(25, 0), {5, 5});

	uiButton b0({300, 200, 700, 600}, scaleMode::scaleXY, {"src0.png"}, {"src1.png"}, [&sel, &t0](sf::Vector2f p) {
		int ix = p.x * 5, iy = p.y * 5;
		ix = ix < 0 ? 0 : ix > 4 ? 4 : ix;
		iy = iy < 0 ? 0 : iy > 4 ? 4 : iy;
		int i = iy * 5 + ix;
		//printf("(%f %f) -> (%d %d) -> %d\n", p.x, p.y, ix, iy, i);
		int s = sel.getSelected();
		if (s != -1)t0.setByIndex(i, s);
	});
	g0.setUIPart(0, b0);
	g0.setUIPart(1, t0);
	g0.setUIPart(2, selGrid);
	subCut.setUIPart(0, g0);

	mainWindow.setUIScene(subCut);

//	uiSideCut sc({1, 1});
//	uiGroup g0({0, 0, 800, 800});
//	uiImage img({0, 0, 200, 200}, scaleMode::scaleXY, {"src0.png"});
//	sc.setUIPart(0, g0);
//	g0.setUIPart(0, img);
//	mainWindow.setUIScene(sc);
	mainWindow.startRenderCycle();

	return 0;
}