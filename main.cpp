#include <cstdio>
#include "window.h"
#include <memory>
#include "connection.h"
#include <SFML/Network.hpp>
#include <utility>

//#define DEMO0
//#define DEMO1
#define DEMO2

class asyncPrinter {
	static void liveCycle(asyncPrinter* p) {
		while (p->s == PRINTING) {
			for (int i = 0; i < 100; i++) {
				std::cout << "*";
			}
			std::cout << "\n";
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	std::unique_ptr<std::thread> th;
public:
	enum state {
		PREPARE,
		PRINTING,
		STOP
	} s = PREPARE;
	asyncPrinter() {
		s = PRINTING;
		th = std::make_unique<std::thread>(liveCycle, this);
	}
	~asyncPrinter() {
		s = STOP;
		th->join();
	}
};

int main() {
#ifdef DEMO0
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
		int s = sel.getSelected();
		if (s != -1)t0.setByIndex(i, s);
	});
	g0.setUIPart(0, b0);
	g0.setUIPart(1, t0);
	g0.setUIPart(2, selGrid);
	subCut.setUIPart(0, g0);

	window mainWindow("~~~", {1200, 800});
	mainWindow.setUIScene(subCut);

	mainWindow.startRenderCycle();
#endif
#ifdef DEMO1
	uiImage img0({0, 0, 800, 800}, scaleMode::scaleXY, {"src0.png"});
	std::string text0s;
	uiText text0({0, 0, 800, 300}, scaleMode::scaleXY, "", "Ninja Naruto.ttf");

	uiGroup mainGroup({0, 0, 800, 800}, scaleMode::fullZone);
	mainGroup.addUIPart(img0);
	mainGroup.addUIPart(text0);

	uiSideCut sideCut({1, 1});
	sideCut.setUIPart(0, mainGroup);

	window mainWindow("~~~", {1200, 800});
	mainWindow.setUIScene(sideCut);
	mainWindow.addKeyEvent([&text0s, &text0](uint32_t x) {
		if ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || x == '_' || x == ' ') {
			text0s += (char)x;
			text0.setString(text0s);
		}
	});

	mainWindow.startRenderCycle();
#endif
#ifdef DEMO2
	struct dataG_t {
		enum ctype_t {
			SERVER,
			CLIENT
		} ctype;
	} dataG{};
	struct data1_t {
		std::shared_ptr<letterSenderServer> ls;
		std::thread th;

		enum state {
			PREPARE,
			OK,
			ERROR
		} s = PREPARE;
		static void lifeCycle(data1_t* data, uiText* display, uiScene* s1, uiScene* s2) {
			bool w = true;
			while (w) {
				switch (data->ls->s) {
					case letterSender::GOTIP: {
						display->setString(data->ls->getIPnPort());
					}
					case letterSender::START:
					case letterSender::CONNECTING:break;
					case letterSender::MAILING: {
						printf("connected ^_^\n");
						s1->changeToNewScene(*s2);
						data->s = OK;
						return;
					}
					case letterSender::ERROR:
					case letterSender::STOP: {
						w = false;
					}
				}
				if (data->s == ERROR) break;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			printf("error ._.\n");
		}
		explicit data1_t(uiText& ipText, uiScene& s1, uiScene& s2) :ls(std::make_shared<letterSenderServer>()) {
			th = std::thread(lifeCycle, this, &ipText, &s1, &s2);
		}
		~data1_t() {
			ls->s = letterSender::STOP;
			th.join();
		}
	};
	struct data2_t {
		std::shared_ptr<letterSenderClient> ls;
		std::string ipbuf;
		std::thread th;
		enum state {
			PREPARE,
			OK,
			ERROR
		} s = PREPARE;
		static void lifeCycle(data2_t* data, uiScene* s1, uiScene* s2) {
			bool w = true;
			while (w) {
				switch (data->ls->s) {
					case letterSender::GOTIP:
					case letterSender::START:
					case letterSender::CONNECTING:break;
					case letterSender::MAILING: {
						printf("connected ^_^\n");
						s1->changeToNewScene(*s2);
						data->s = OK;
						return;
					}
					case letterSender::ERROR:
					case letterSender::STOP: {
						w = false;
					}
				}
				if (data->s == ERROR) break;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			printf("error ._.\n");
		}
		void start(uiScene& s1, uiScene& s2) {
			ls = std::make_shared<letterSenderClient>(ipbuf);
			th = std::thread(lifeCycle, this, &s1, &s2);
		}
		~data2_t() {
			if (ls) {
				ls->s = letterSender::STOP;
				s = ERROR;
				th.join();
			}
		}
	};
	struct data3_t {
		std::string msgbuf;
		std::shared_ptr<letterSender> ls;
		std::thread th;

		enum state {
			MAILING,
			STOP
		} s = MAILING;

		static void lifeCycle(data3_t* data, uiText* display) {
			while (data->s==MAILING) {
				display->setString(data->ls->getmsg());
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		explicit data3_t(std::shared_ptr<letterSender> ls, uiText& display) :ls(std::move(ls)) {
			th = std::thread(lifeCycle, this, &display);
		}
		~data3_t() {
			s = STOP;
			th.join();
		}
	};
	std::unique_ptr<data1_t> data1 = nullptr;
	std::unique_ptr<data2_t> data2 = nullptr;
	std::unique_ptr<data3_t> data3 = nullptr;

	uiGroup mainGroup(box2::unit(), scaleMode::fullZone);
	window mainWindow("@_@", {1000, 1000});
	//"WELCOME" SCENE
	uiButton b_serv({50, 400, 450, 800}, scaleMode::scaleXY, {"src0.png"}, {"src1.png"});
	uiButton b_client({550, 400, 950, 800}, scaleMode::scaleXY, {"src0.png"}, {"src1.png"});
	//
	uiScene welcomeScene(true, {0, 0, 1000, 1000});
	mainGroup.addUIPart(welcomeScene);
	welcomeScene.addUIPart(b_client);
	welcomeScene.addUIPart(b_serv);
	welcomeScene.addUIPart2(new uiText({50, 850, 950, 950}, scaleMode::scaleXY, "select mode:", "Ninja Naruto.ttf"));
	welcomeScene.addUIPart2(new uiText({50, 400, 450, 800}, scaleMode::scaleXY, "server", "Ninja Naruto.ttf"));
	welcomeScene.addUIPart2(new uiText({550, 400, 950, 800}, scaleMode::scaleXY, "client", "Ninja Naruto.ttf"));
	//"SERVER CONNECTION" SCENE
	uiText t_ip({50, 850, 950, 950}, scaleMode::scaleXY, "...", "Ninja Naruto.ttf");
	//
	uiScene serverConnectScene(false, {0, 0, 1000, 1000});
	mainGroup.addUIPart(serverConnectScene);
	serverConnectScene.addUIPart(t_ip);
	b_serv.setAction(uiScene::switchScene(welcomeScene, serverConnectScene));
	//"CLIENT CONNECTION" SCENE
	uiText t_ipInput({50, 850, 950, 950}, scaleMode::scaleXY, "", "Ninja Naruto.ttf");
	//
	uiScene clientConnectScene(false, {0, 0, 1000, 1000});
	clientConnectScene.addUIPart(t_ipInput);
	mainGroup.addUIPart(clientConnectScene);
	b_client.setAction(uiScene::switchScene(welcomeScene, clientConnectScene));
	clientConnectScene.setOnOpenAction([&data2, &dataG]() {
		dataG.ctype = dataG_t::CLIENT;
		data2 = std::make_unique<data2_t>();
	});
	//MAILING SCENE
	uiText t_msgin({50, 850, 950, 950}, scaleMode::scaleXY, "[.....]", "Ninja Naruto.ttf");
	uiText t_msgout({50, 600, 950, 700}, scaleMode::scaleXY, ">", "Ninja Naruto.ttf");
	//
	uiScene mailingScene(false, {0, 0, 1000, 1000});
	mailingScene.addUIPart(t_msgin);
	mailingScene.addUIPart(t_msgout);
	mainGroup.addUIPart(mailingScene);
	serverConnectScene.setOnOpenAction([&data1, &t_ip, &dataG, &serverConnectScene, &mailingScene]() {
		dataG.ctype = dataG_t::SERVER;
		data1 = std::make_unique<data1_t>(t_ip, serverConnectScene, mailingScene);
	});
	mainWindow.addKeyEvent([&clientConnectScene, &t_ipInput, &data2, &mailingScene](uint32_t i) {
		if (clientConnectScene.isActive()) {
			if (i == 13) {
				printf("[%s]\n", data2->ipbuf.c_str());
				data2->start(clientConnectScene, mailingScene);
			} else if ((i >= '0' && i <= '9') || i == '.' || i == ' ') {
				data2->ipbuf += (char)i;
				t_ipInput.setString(data2->ipbuf);
			}
		}
	});
	mailingScene.setOnOpenAction([&dataG, &data1, &data2, &data3, &t_msgin]() {
		if (dataG.ctype == dataG_t::SERVER) {
			data3 = std::make_unique<data3_t>(data1->ls, t_msgin);
		} else {
			data3 = std::make_unique<data3_t>(data2->ls, t_msgin);
		}
	});
	mainWindow.addKeyEvent([&mailingScene, &data3, &t_msgout](uint32_t i) {
		if (mailingScene.isActive()) {
			if (i == 13) {
				data3->ls->send(data3->msgbuf);
				data3->msgbuf = "";
			} else if (i >= ' ' && i <= 'Z') {
				data3->msgbuf += (char)i;
			}
			t_msgout.setString(data3->msgbuf);
		}
	});
	//FINAL
	mainWindow.setUIScene(mainGroup);
	mainWindow.startRenderCycle();
#endif
	return 0;
}