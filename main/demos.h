#pragma once
#include <utility>

#include "../ui/Window.h"
#include "../specials/specials.h"
#include "../chess/chess.h"

namespace demo0 {
	void Demo() {
		using namespace ui;
		Window mainWin("\\@_@/", {1000, 1000}, 2);

		const int SCENE_0 = 0, SCENE_1 = 1;
		class MainManager : public AppManager {
			void OnBtnClick(uint32_t id, sf::Vector2f pos) override {
				uint32_t snid = (currentScene << 16) | id;
				switch (snid) {
					case (SCENE_0 << 16) | 0: {
						SwitchScene(SCENE_1);
						break;
					}
					case (SCENE_1 << 16) | 0: {
						SwitchScene(SCENE_0);
						break;
					}
					default: assert(false);
				}
				Unblock();
			}
		public:
			explicit MainManager(Window& w) :AppManager(w) { }
		} manager(mainWin);

		//SCENE_0
		auto btn00 = new Button({0, 800, 500, 1000}, ScaleMode::scaleXY, {"src2.png"}, {"src3.png"}, 0);
		//
		auto scene0 = new Group({0, 0, 1000, 1000}, ScaleMode::fullZone);
		scene0->AddUIPart(new Image({500, 200, 1000, 1000}, ScaleMode::scaleXY, {"src1.png"}));
		scene0->AddUIPart(new Image({0, 0, 500, 800}, ScaleMode::scaleXY, {"src0.png"}));
		scene0->AddUIPart(btn00);
		//SCENE_1
		auto btn10 = new Button({500, 0, 1000, 200}, ScaleMode::scaleXY, {"src2.png"}, {"src3.png"}, 0);
		//
		auto scene1 = new Group({0, 0, 1000, 1000}, ScaleMode::fullZone);
		scene1->AddUIPart(new Image({500, 200, 1000, 1000}, ScaleMode::scaleXY, {"src0.png"}));
		scene1->AddUIPart(new Image({0, 0, 500, 800}, ScaleMode::scaleXY, {"src1.png"}));
		scene1->AddUIPart(btn10);
		//FINAL
		mainWin.SetScene(scene0, SCENE_0);
		mainWin.SetScene(scene1, SCENE_1);
		mainWin.SetManager(manager);

		mainWin.StartRenderCycle();
		mainWin.Wait();
	}
}

namespace demo1 {
	uint32_t sprIdByFigure(Figure f) {
		Figure ft = f & figures::MASKF;
		Figure fc = f & figures::MASKC;
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

	void Demo() {
		using namespace ui;
		Window mainWin("~_~", {1000, 800}, 1);
		const int SCENE_0 = 0;
		const sf::Vector2u gridMetrics = {8, 8};
		const sf::Vector2u varGridMetrics = {2, 7};
		Board mainBoard(Board::EMPTY);

		std::vector<Figure> varList = {
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
		auto scene0 = new Group({0, 0, 1000, 800}, ScaleMode::bindTL);
		mainWin.SetScene(scene0, SCENE_0);

		auto grid = new Group({0, 100, 700, 800}, ScaleMode::bindTL);
		auto grid_tm = new TileMap(Box2::unit(), ScaleMode::fullZone, "chesstilemap.png", {16, 16});
		grid_tm->SetIndexes(std::vector<uint32_t>(gridMetrics.x * gridMetrics.y, sprIdByFigure(figures::NONE)), gridMetrics);
		auto grid_btn = new InvisibleButton(Box2::unit(), ScaleMode::fullZone, 0);
		grid->AddUIPart(grid_tm);
		grid->AddUIPart(grid_btn);

		auto evalText = new Text({0, 0, 700, 100}, ScaleMode::bindTL, "Eval:.....", "Ninja Naruto.ttf");
		auto evalBtn = new Button({800, 0, 1000, 100}, ScaleMode::bindTL, {"src0.png"}, {"src1.png"}, 1);

		auto varGrid = new Group({800, 100, 1000, 800}, ScaleMode::bindTL);
		auto varGrid_tm = new TileMap(Box2::unit(), ScaleMode::fullZone, "chesstilemap.png", {16, 16});
		varGrid_tm->SetIndexes(varListU32, varGridMetrics);
		auto varGrid_sel = new SelectionTM(varGrid_tm, {"src4.png"});
		auto varGrid_btn = new InvisibleButton(Box2::unit(), ScaleMode::fullZone, 2);

		varGrid->AddUIPart(varGrid_tm);
		varGrid->AddUIPart(varGrid_sel);
		varGrid->AddUIPart(varGrid_btn);

		scene0->AddUIPart(evalBtn);
		scene0->AddUIPart(grid);
		scene0->AddUIPart(evalText);
		scene0->AddUIPart(varGrid);

		AppManagerDefault manager(mainWin);
		manager.obc = [&](uint32_t id, sf::Vector2f pos, AppManager* self) {
			uint32_t snid = (self->currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					auto ipos = grid_tm->Proj(pos);
					auto selid = varGrid_sel->getSelPos();
					if (cm::Valid(ipos, gridMetrics) && cm::Valid(selid, varGridMetrics)) {
						uint32_t I = selid.x + selid.y * varGridMetrics.x;
						grid_tm->SetByIndex(ipos.x + ipos.y * gridMetrics.x,
								varListU32[I]);
						mainBoard.At(ipos.x, ipos.y) = varList[I];
					}
					break;
				}
				case (SCENE_0 << 16) | 1: {
					evalText->SetString("Eval: " + std::to_string(mainBoard.Eval()));
					break;
				}
				case (SCENE_0 << 16) | 2: {
					auto ipos = varGrid_tm->Proj(pos);
					if (cm::Valid(ipos, gridMetrics)) {
						varGrid_sel->Click(ipos);
					}
					break;
				}
				default: assert(false);
			}
			self->Unblock();
		};

		//FINAL
		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();
		mainWin.Wait();
	}
}
namespace demo2 {
	void Demo() {
		ui::Window mainWin("( ._. )/", {1000, 800}, 1);
		auto shader = new ui::Shader({100, 100, 200, 500}, ui::ScaleMode::scaleXY, "shader0.frag");
		mainWin.SetScene(shader, 0);

		enum State {
			RENDER,
			STOP
		} state = RENDER;

		ui::AppManagerDefault manager(mainWin);
		manager.oe = [&](ui::AppManager* self) {
			self->AppManager::OnExit();
			state = STOP;
		};

		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();

		sf::Clock clock;
		while (state == RENDER) {
			shader->SetTime(clock.getElapsedTime().asSeconds());
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		mainWin.Wait();
	}
}