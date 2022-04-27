#pragma once
#include <utility>

#include "../ui/Window.h"
#include "../specials/specials.h"
#include "../chess/chess.h"
#include "../ui/formparser.h"
#include "../pinboard/pinboard.h"

namespace demo0 {
	void Demo() {
		using namespace ui;
		Window mainWin("\\@_@/", {1000, 1000}, 2);

		const int SCENE_0 = 0, SCENE_1 = 1;
		AppManager manager(mainWin);
		manager.obc = [&](AppManager* self, uint32_t id, sf::Vector2f pos) {
			uint32_t snid = (self->currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					self->SwitchScene(SCENE_1);
					break;
				}
				case (SCENE_1 << 16) | 0: {
					self->SwitchScene(SCENE_0);
					break;
				}
				default: assert(false);
			}
			self->Unblock();
		};
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
		auto grid = new Group({0, 100, 700, 800}, ScaleMode::bindTL);
		scene0->AddUIPart(grid);
		auto grid_tm = new TileMap(Box2::Unit(), ScaleMode::fullZone, "chesstilemap.png", {16, 16});
		grid->AddUIPart(grid_tm);
		auto grid_btn = new InvisibleButton(Box2::Unit(), ScaleMode::fullZone, 0);
		grid->AddUIPart(grid_btn);
		auto evalText = new Text({0, 0, 700, 100}, ScaleMode::bindTL, "Eval:.....", "Ninja Naruto.ttf");
		scene0->AddUIPart(evalText);
		auto evalBtn = new Button({800, 0, 1000, 100}, ScaleMode::bindTL, {"src0.png"}, {"src1.png"}, 1);
		scene0->AddUIPart(evalBtn);
		auto varGrid = new Group({800, 100, 1000, 800}, ScaleMode::bindTL);
		scene0->AddUIPart(varGrid);
		auto varGrid_tm = new TileMap(Box2::Unit(), ScaleMode::fullZone, "chesstilemap.png", {16, 16});
		varGrid->AddUIPart(varGrid_tm);
		auto varGrid_sel = new SelectionTM(varGrid_tm, {"src4.png"});
		varGrid->AddUIPart(varGrid_sel);
		auto varGrid_btn = new InvisibleButton(Box2::Unit(), ScaleMode::fullZone, 2);
		varGrid->AddUIPart(varGrid_btn);

		mainWin.SetScene(scene0, SCENE_0);

		grid_tm->SetIndexes(std::vector<uint32_t>(gridMetrics.x * gridMetrics.y, sprIdByFigure(figures::NONE)), gridMetrics);
		varGrid_tm->SetIndexes(varListU32, varGridMetrics);

		AppManager manager(mainWin);
		manager.obc = [&](AppManager* self, uint32_t id, sf::Vector2f pos) {
			uint32_t snid = (self->currentScene << 16) | id;
			switch (snid) {
				case (SCENE_0 << 16) | 0: {
					auto ipos = grid_tm->Proj(pos);
					auto selid = varGrid_sel->GetSelPos();
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

		ui::AppManager manager(mainWin);
		manager.oe = [&](ui::AppManager* self) {
			self->Close();
			state = STOP;
		};

		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();

		sf::Clock clock;
		while (state == RENDER) {
			shader->SetTime(clock.getElapsedTime().asSeconds());
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}
namespace demo3 {
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
		Window mainWin("| _ |", {1000, 800}, 1);

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

		Parser parser;
		parser.Parse("demo1", mainWin);

		auto grid_tm = (TileMap*)parser["scene0_grid_tm"];
		auto varGrid_tm = (TileMap*)parser["scene0_varGrid_tm"];
		auto varGrid_sel = (SelectionTM*)parser["scene0_varGrid_tm_sel"];
		auto evalText = (Text*)parser["scene0_evalText"];

		AppManager manager(mainWin);
		manager.obc = [&](AppManager* self, uint32_t id, sf::Vector2f pos) {
			uint32_t snid = (self->currentScene << 16) | id;
			switch (snid) {
				case (0 << 16) | 0: {
					auto ipos = grid_tm->Proj(pos);
					auto selid = varGrid_sel->GetSelPos();
					if (cm::Valid(ipos, gridMetrics) && cm::Valid(selid, varGridMetrics)) {
						uint32_t I = selid.x + selid.y * varGridMetrics.x;
						grid_tm->SetByIndex(ipos.x + ipos.y * gridMetrics.x,
								varListU32[I]);
						mainBoard.At(ipos.x, ipos.y) = varList[I];
					}
					break;
				}
				case (0 << 16) | 1: {
					evalText->SetString("Eval: " + std::to_string(mainBoard.Eval()));
					break;
				}
				case (0 << 16) | 2: {
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

		grid_tm->SetIndexes(std::vector<uint32_t>(gridMetrics.x * gridMetrics.y, sprIdByFigure(figures::NONE)), gridMetrics);;
		varGrid_tm->SetIndexes(varListU32, varGridMetrics);

		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();
	}
}
namespace demo4 {
	void Demo() {
		using namespace ui;
		Window mainWin("<|* _ *|>", {1000, 800}, 1);

		auto* img = new ui::Image({0, 0, 1000, 800}, ScaleMode::fullZone, {"chesstilemap.png", {16, 16}, 32, Sprite::Param::SCALE9, {100, 100}});

		mainWin.SetScene(img, 0);

		AppManager manager(mainWin);

		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();
	}
}
namespace demo5 {
	uint32_t GetTextureByPin(Pin p) {
		return p;
	}
	std::vector<uint32_t> GetTexturesByPins(const std::vector<Pin>& pins) {
		uint32_t n = pins.size();
		std::vector<uint32_t> ans(n);
		for (uint32_t i = 0; i < n; i++) {
			ans[i] = GetTextureByPin(pins[i]);
		}
		return ans;
	}

	void Demo() {
		using namespace ui;
		Window mainWin("<|* _ *|>", {1500, 900}, 1);

		Parser parser;
		parser.Parse("botdemo", mainWin);

		auto board = (ui::TileMap*)parser["scene0_board_tm"];
		auto var = (ui::Variant*)parser["scene0_v1"];

		Pinboard pinboard(10);
		board->SetIndexes(GetTexturesByPins(pinboard.GetData()), {10, 10});
		enum State {
			PLAYER,
			BOT,
			END
		} state = PLAYER;

		AppManager manager(mainWin);
		manager.obc = [&](AppManager* self, uint32_t id, sf::Vector2f pos) {
			if (id == 0) {
				self->OnExit();
			} else if (id == 1) {
				if (state == PLAYER) {
					auto p = board->Proj(pos);
					pinboard.step(p.x, p.y, rand() % pins::MAX);
					board->SetIndexes(GetTexturesByPins(pinboard.GetData()));
					state = BOT;
					var->SwitchTo(1);
				}
			}
			self->Unblock();
		};
		manager.oe = [&](AppManager* self) {
			pinboard.SetState(Pinboard::END);
			state = END;
			self->Close();
		};

		mainWin.SetManager(manager);
		mainWin.StartRenderCycle();

		while (state != END) {
			switch (state) {
				case PLAYER: {
					break;
				}
				case BOT: {
					if (pinboard.GetState() == Pinboard::PLAYER) {
						board->SetIndexes(GetTexturesByPins(pinboard.GetData()));
						state = PLAYER;
						var->SwitchTo(0);
					}
					break;
				}
				default:break;
			}
		}
	}
}