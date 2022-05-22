#pragma once
#include <utility>

#include "../utils/utils.h"
#include "../chess/chess.h"
#include "../ui/formparser.h"
#include "../pinboard/pinboard.h"

namespace demo0 {
	void Demo() {
		using namespace ui;
		Window mainWin("\\@_@/", {1000, 1000}, 2);

		const int SCENE_0 = 0, SCENE_1 = 1;
		AppManager manager(mainWin);
		manager.obc = [&](AppManager* self, uint32_t id, sf::Vector2f) {
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
		};
		//SCENE_0
		auto btn00 = new Button({0, 800, 500, 1000}, ScaleMode::scaleXY, {"resources/src2.png"}, {"resources/src3.png"}, 0);
		//
		auto scene0 = new Group({0, 0, 1000, 1000}, ScaleMode::fullZone);
		scene0->AddUIPart(new Image({500, 200, 1000, 1000}, ScaleMode::scaleXY, {"resources/src1.png"}));
		scene0->AddUIPart(new Image({0, 0, 500, 800}, ScaleMode::scaleXY, {"resources/src0.png"}));
		scene0->AddUIPart(btn00);
		//SCENE_1
		auto btn10 = new Button({500, 0, 1000, 200}, ScaleMode::scaleXY, {"resources/src2.png"}, {"resources/src3.png"}, 0);
		//
		auto scene1 = new Group({0, 0, 1000, 1000}, ScaleMode::fullZone);
		scene1->AddUIPart(new Image({500, 200, 1000, 1000}, ScaleMode::scaleXY, {"resources/src0.png"}));
		scene1->AddUIPart(new Image({0, 0, 500, 800}, ScaleMode::scaleXY, {"resources/src1.png"}));
		scene1->AddUIPart(btn10);
		//FINAL
		mainWin.SetScene(scene0, SCENE_0);
		mainWin.SetScene(scene1, SCENE_1);

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

		for (uint32_t i = 0; i < varGridMetrics.x * varGridMetrics.y; i++) {
			varListU32[i] = sprIdByFigure(varList[i]);
		}

		auto scene0 = new Group({0, 0, 1000, 800}, ScaleMode::bindTL);
		auto grid = new Group({0, 100, 700, 800}, ScaleMode::bindTL);
		scene0->AddUIPart(grid);
		auto grid_tm = new TileMap(Box2::Unit(), ScaleMode::fullZone, "resources/chesstilemap.png", {16, 16});
		grid->AddUIPart(grid_tm);
		auto grid_btn = new InvisibleButton(Box2::Unit(), ScaleMode::fullZone, 0);
		grid->AddUIPart(grid_btn);
		auto evalText = new Text({0, 0, 700, 100}, ScaleMode::bindTL, "Eval:.....", "resources/font2.ttf");
		scene0->AddUIPart(evalText);
		auto evalBtn = new Button({800, 0, 1000, 100}, ScaleMode::bindTL, {"resources/src0.png"}, {"resources/src1.png"}, 1);
		scene0->AddUIPart(evalBtn);
		auto varGrid = new Group({800, 100, 1000, 800}, ScaleMode::bindTL);
		scene0->AddUIPart(varGrid);
		auto varGrid_tm = new TileMap(Box2::Unit(), ScaleMode::fullZone, "resources/chesstilemap.png", {16, 16});
		varGrid->AddUIPart(varGrid_tm);
		auto varGrid_sel = new SelectionTM(varGrid_tm, {"resources/src4.png"});
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
					evalText->SetString("Eval: " + std::to_string(0));
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
		};

		//FINAL
		mainWin.StartRenderCycle();
	}
}
namespace demo2 {
	void Demo() {
		ui::Window mainWin("( ._. )/", {1000, 800}, 1);
		auto shader = new ui::Shader({100, 100, 200, 500}, ui::ScaleMode::scaleXY, "resources/shader0.frag");
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

		mainWin.StartRenderCycle();

		sf::Clock clock;
		while (state == RENDER) {
			shader->SetUniform("u_time", clock.getElapsedTime().asSeconds());
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

		for (uint32_t i = 0; i < varGridMetrics.x * varGridMetrics.y; i++) {
			varListU32[i] = sprIdByFigure(varList[i]);
		}

		Parser parser;
		parser.Parse("resources/demo1", mainWin);

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
					evalText->SetString("Eval: " + std::to_string(0));
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
		};

		grid_tm->SetIndexes(std::vector<uint32_t>(gridMetrics.x * gridMetrics.y, sprIdByFigure(figures::NONE)), gridMetrics);;
		varGrid_tm->SetIndexes(varListU32, varGridMetrics);

		mainWin.StartRenderCycle();
	}
}
namespace demo4 {
	void Demo() {
		using namespace ui;
		Window mainWin("<|* _ *|>", {1000, 800}, 1);

		auto* img = new ui::Image({0, 0, 1000, 800}, ScaleMode::fullZone, {"resources/chesstilemap.png", {16, 16}, 32, Sprite::Param::SCALE9, {100, 100}});

		mainWin.SetScene(img, 0);

		AppManager manager(mainWin);
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
		parser.Parse("resources/botdemo", mainWin);

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
		};
		manager.oe = [&](AppManager* self) {
			pinboard.SetState(Pinboard::END);
			state = END;
			self->Close();
		};

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
namespace demo6 {
	void Demo() {
		using namespace ui;
		Window mainWin("zoom demo", {1500, 900}, 1);

		Parser parser;
		parser.Parse("resources/zoomdemo", mainWin);

		auto shader = (ui::Shader*)parser["g0_sh0"];

		enum State {
			WORK,
			END
		} state = WORK;

		AppManager manager(mainWin);
		manager.oe = [&](AppManager* self) {
			state = END;
			self->Close();
		};
		shader->SetUniform("u_src", "resources/src0.png");
		mainWin.StartRenderCycle();

		sf::Clock clock;
		while (state != END) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			auto pos = mainWin.GetMousePos();
			shader->SetUniform("u_mouse", pos);
			shader->SetUniform("u_time", clock.getElapsedTime().asSeconds());
		}
	}
}
namespace demo7 {
	const uint32_t SIZE = 8;

	std::vector<uint32_t> BackIds(uint32_t idW, uint32_t idB) {
		std::vector<uint32_t> ans(SIZE * SIZE);
		for (uint32_t y = 0, i = 0, x; y < SIZE; y++) {
			for (x = 0; x < SIZE; x++, i++) {
				ans[i] = (x + y) % 2 ? idW : idB;
			}
		}
		return ans;
	}
	uint32_t ToTexId(Figure f) {
		Figure ft = f & figures::MASKF;
		Figure fc = f & figures::MASKC;
		uint32_t rt = ft == figures::PAWN ? 0 :
					  ft == figures::BISHOP ? 1 :
					  ft == figures::KNIGHT ? 2 :
					  ft == figures::ROOK ? 3 :
					  ft == figures::QUEEN ? 4 :
					  ft == figures::KING ? 5 : 15;
		uint32_t rc = fc == figures::WHITE ? 0 : fc == figures::BLACK ? 1 : 15;
		return rt + rc * 16;
	}
	std::vector<uint32_t> GetIds(const Board& board) {
		std::vector<uint32_t> ans(SIZE * SIZE);
		for (uint32_t y = 0, i = 0, x; y < SIZE; y++) {
			for (x = 0; x < SIZE; x++, i++) {
				ans[i] = ToTexId(board.At(x, y));
			}
		}
		return ans;
	}
	void Demo() {
		using namespace ui;

		Board board(Board::DEFAULT);

		Window mainWin("chess?", {1200, 900}, 1);
		Parser parser;
		parser.Parse("resources/chessform", mainWin);

		auto back = (TileMap*)parser["s0_board_back"];
		auto front = (TileMap*)parser["s0_board_front"];
		//auto gridbtn = (InvisibleButton*)parser["s0_board_btn"];
		auto front_sel = (SelectionTM*)parser["s0_board_front_sel"];
		//auto btn_rst = (Button*)parser["s0_rst_b"];

		AppManager manager(mainWin);
		back->SetIndexes(BackIds(14, 15), {8, 8});
		front->SetIndexes(GetIds(board), {8, 8});

		sf::Vector2i selpos = {-1, -1};
		manager.obc = [&](AppManager*, uint32_t id, sf::Vector2f pos) {
			switch (id) {
				case 0: {
					auto ipos = front->Proj(pos);
					if (!cm::Valid(ipos, sf::Vector2i(SIZE, SIZE))) break;
					if (ipos == selpos) {
						selpos = {-1, -1};
					} else if (cm::Valid(selpos, sf::Vector2i(SIZE, SIZE))) {
						if (board.PlayerStep(selpos, ipos)) {
							front->SetIndexes(GetIds(board), {8, 8});
							selpos = {-1, -1};
						} else {
						}
					} else {
						selpos = ipos;
					}
					front_sel->Select(selpos);
					break;
				}
				case 1: {
					board.Reset();
					front->SetIndexes(GetIds(board), {8, 8});
					break;
				}
				default : assert(false);
			}
		};

		mainWin.StartRenderCycle();
	}
}