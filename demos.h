#pragma once
#include "window.h"

void demo0() {
	window mainWin("\\@_@/", {1000, 1000}, 2);

	const int SCENE_0 = 0, SCENE_1 = 1;
	class mainManager : public appManager {
		void onBtnClick(uint32_t id, sf::Vector2f pos) override {
			uint32_t snid = (currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					switchScene(SCENE_1);
					break;
				}
				case (SCENE_1 << 16) | 0: {
					switchScene(SCENE_0);
					break;
				}
				default: assert(false);
			}
			unblock();
		}
	public:
		explicit mainManager(window& w) :appManager(w) { }
	} manager(mainWin);

	//SCENE_0
	auto btn00 = new uiButton({0, 800, 500, 1000}, scaleMode::scaleXY, {"src2.png"}, {"src3.png"}, 0);
	//
	uiGroup scene0({0, 0, 1000, 1000}, scaleMode::fullZone);
	scene0.addUIPart(new uiImage({500, 200, 1000, 1000}, scaleMode::scaleXY, {"src1.png"}));
	scene0.addUIPart(new uiImage({0, 0, 500, 800}, scaleMode::scaleXY, {"src0.png"}));
	scene0.addUIPart(btn00);
	//SCENE_1
	auto btn10 = new uiButton({500, 0, 1000, 200}, scaleMode::scaleXY, {"src2.png"}, {"src3.png"}, 0);
	//
	uiGroup scene1({0, 0, 1000, 1000}, scaleMode::fullZone);
	scene1.addUIPart(new uiImage({500, 200, 1000, 1000}, scaleMode::scaleXY, {"src0.png"}));
	scene1.addUIPart(new uiImage({0, 0, 500, 800}, scaleMode::scaleXY, {"src1.png"}));
	scene1.addUIPart(btn10);
	//FINAL
	mainWin.setScene(scene0, SCENE_0);
	mainWin.setScene(scene1, SCENE_1);
	mainWin.setManager(manager);

	mainWin.startRenderCycle();
	mainWin.wait();
}

void demo1() {
	window mainWin("~_~", {1200, 1000}, 1);
	const int SCENE_0 = 0;
	//SCENE_0
	uiSideCut scene0({1, 1});
	mainWin.setScene(scene0, SCENE_0);
	auto grid = new uiGroup();
	auto grid_btn = new uiInvisibleButton(box2::unit(), scaleMode::fullZone, 0);
	auto grid_tm = new uiTilemap(box2::unit(), scaleMode::fullZone, "chesstilemap.png", {16, 16}, 0, 32);
	grid_tm->setIndexes({0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3}, {4, 4});
	grid->addUIPart(grid_tm);
	grid->addUIPart(grid_btn);
	scene0.setPart(grid, 0);

	class mainManager : public appManager {
		void onBtnClick(uint32_t id, sf::Vector2f pos) override {
			uint32_t snid = (currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					pos = uiels[0]->toUnit(pos);
					printf("%d %d\n", (int)(pos.x*4), (int)(pos.y * 4));
					break;
				}
				default: assert(false);
			}
			unblock();
		}
	public:
		explicit mainManager(window& w) :appManager(w) { }
	} manager(mainWin);
	manager.setUiPartsPtrs({grid_btn});

	//FINAL
	mainWin.setManager(manager);
	mainWin.startRenderCycle();
	mainWin.wait();
}