#pragma once
#include <utility>

#include "window.h"
#include "specials.h"
#include "chess.h"

namespace demo0 {
	void demo() {
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
		auto scene0 = new uiGroup({0, 0, 1000, 1000}, scaleMode::fullZone);
		scene0->addUIPart(new uiImage({500, 200, 1000, 1000}, scaleMode::scaleXY, {"src1.png"}));
		scene0->addUIPart(new uiImage({0, 0, 500, 800}, scaleMode::scaleXY, {"src0.png"}));
		scene0->addUIPart(btn00);
		//SCENE_1
		auto btn10 = new uiButton({500, 0, 1000, 200}, scaleMode::scaleXY, {"src2.png"}, {"src3.png"}, 0);
		//
		auto scene1 = new uiGroup({0, 0, 1000, 1000}, scaleMode::fullZone);
		scene1->addUIPart(new uiImage({500, 200, 1000, 1000}, scaleMode::scaleXY, {"src0.png"}));
		scene1->addUIPart(new uiImage({0, 0, 500, 800}, scaleMode::scaleXY, {"src1.png"}));
		scene1->addUIPart(btn10);
		//FINAL
		mainWin.setScene(scene0, SCENE_0);
		mainWin.setScene(scene1, SCENE_1);
		mainWin.setManager(manager);

		mainWin.startRenderCycle();
		mainWin.wait();
	}
}

namespace demo1 {
	uint32_t sprIdByFigure(figure f) {
		figure ft = f & figures::MASKF;
		figure fc = f & figures::MASKC;
		int it = (ft == figures::PAWN) ? 0 :
				 (ft == figures::BISHOP) ? 1 :
				 (ft == figures::KNIGHT) ? 2 :
				 (ft == figures::ROOK) ? 3 :
				 (ft == figures::QUEEN) ? 4 :
				 (ft == figures::KING) ? 5 : 15;
		int ic = (fc == figures::WHITE) ? 0 :
				 (fc == figures::BLACK) ? 1 : 15;
		return ic * 16 + it;
	}

	void demo() {
		window mainWin("~_~", {1000, 800}, 1);
		const int SCENE_0 = 0;
		const sf::Vector2u gridMetrics = {8, 8};
		const sf::Vector2u varGridMetrics = {2, 7};
		board mainBoard(board::EMPTY);

		std::vector<figure> varList = {
				figures::PAWN | figures::WHITE, figures::PAWN | figures::BLACK,
				figures::BISHOP | figures::WHITE, figures::BISHOP | figures::BLACK,
				figures::KNIGHT | figures::WHITE, figures::KNIGHT | figures::BLACK,
				figures::ROOK | figures::WHITE, figures::ROOK | figures::BLACK,
				figures::QUEEN | figures::WHITE, figures::QUEEN | figures::BLACK,
				figures::KING | figures::WHITE, figures::KING | figures::BLACK,
				figures::NONE, figures::NONE,
		};
		std::vector<uint32_t> varListU32(varGridMetrics.x * varGridMetrics.y);

		for (int i = 0; i < varGridMetrics.x * varGridMetrics.y; i++) {
			varListU32[i] = sprIdByFigure(varList[i]);
		}
		auto scene0 = new uiGroup({0, 0, 1000, 800}, scaleMode::bindTL);
		mainWin.setScene(scene0, SCENE_0);

		auto grid = new uiGroup({0, 100, 700, 800}, scaleMode::bindTL);
		auto grid_tm = new uiTilemap(box2::unit(), scaleMode::fullZone, "chesstilemap.png", {16, 16});
		grid_tm->setIndexes(std::vector<uint32_t>(gridMetrics.x * gridMetrics.y, sprIdByFigure(figures::NONE)), gridMetrics);
		auto grid_btn = new uiInvisibleButton(box2::unit(), scaleMode::fullZone, 0);
		grid->addUIPart(grid_tm);
		grid->addUIPart(grid_btn);

		auto evalText = new uiText({0, 0, 700, 100}, scaleMode::bindTL, "eval:.....", "Ninja Naruto.ttf");
		auto evalBtn = new uiButton({800, 0, 1000, 100}, scaleMode::bindTL, {"src0.png"}, {"src1.png"}, 1);

		auto varGrid = new uiGroup({800, 100, 1000, 800}, scaleMode::bindTL);
		auto varGrid_tm = new uiTilemap(box2::unit(), scaleMode::fullZone, "chesstilemap.png", {16, 16});
		varGrid_tm->setIndexes(varListU32, varGridMetrics);
		auto varGrid_sel = new uiSelectionTM(varGrid_tm, {"src4.png"});
		auto varGrid_btn = new uiInvisibleButton(box2::unit(), scaleMode::fullZone, 2);

		varGrid->addUIPart(varGrid_tm);
		varGrid->addUIPart(varGrid_sel);
		varGrid->addUIPart(varGrid_btn);

		scene0->addUIPart(evalBtn);
		scene0->addUIPart(grid);
		scene0->addUIPart(evalText);
		scene0->addUIPart(varGrid);

		appManagerDefault manager(mainWin);
		manager.obc = [&](uint32_t id, sf::Vector2f pos, appManager* self) {
			uint32_t snid = (self->currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					auto ipos = grid_tm->proj(pos);
					auto selid = varGrid_sel->getSelPos();
					if (cm::valid(ipos, gridMetrics) && cm::valid(selid, varGridMetrics)) {
						uint32_t I = selid.x + selid.y * varGridMetrics.x;
						grid_tm->setByIndex(ipos.x + ipos.y * gridMetrics.x,
								varListU32[I]);
						mainBoard.at(ipos.x, ipos.y) = varList[I];
					}
					break;
				}
				case (SCENE_0 << 16) | 1: {
					evalText->setString("eval: " + std::to_string(mainBoard.eval()));
					break;
				}
				case (SCENE_0 << 16) | 2: {
					auto ipos = varGrid_tm->proj(pos);
					if (cm::valid(ipos, gridMetrics)) {
						varGrid_sel->click(ipos);
					}
					break;
				}
				default: assert(false);
			}
			self->unblock();
		};

		//FINAL
		mainWin.setManager(manager);
		mainWin.startRenderCycle();
		mainWin.wait();
	}
}
namespace demo2 {
	void demo() {
		window mainWin("( ._. )/", {1000, 800}, 1);
		auto shader = new uiShader({100, 100, 200, 500}, scaleMode::scaleXY, "shader0.frag");
		mainWin.setScene(shader, 0);

		enum state_t {
			RENDER,
			STOP
		} state = RENDER;

		appManagerDefault manager(mainWin);
		manager.obc = [&](uint32_t id, sf::Vector2f pos, appManager* self) {

		};
		manager.oe = [&](appManager* self) {
			self->appManager::onExit();
			state = STOP;
		};

		mainWin.setManager(manager);
		mainWin.startRenderCycle();

		sf::Clock clock;
		while (state == RENDER) {
			shader->setTime(clock.getElapsedTime().asSeconds());
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		mainWin.wait();
	}
}
