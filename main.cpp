#include <cstdio>
#include "window.h"
#include <memory>
#include <thread>
#include <future>
#include <sstream>
#include <SFML/Network.hpp>


//#define DEMO0
//#define DEMO1
#define DEMO2
//#define DEMO3

class letterSender {
protected:
	std::string msgbuf;
	std::mutex mtx;
	sf::TcpSocket socket;
	sf::SocketSelector selector;
public:
	letterSender& operator=(letterSender&) = delete;
	void send(const std::string& msg) {
		if (s != MAILING)return;
		sf::Packet packet;
		packet << msg;
		mtx.lock();
		socket.send(packet);
		mtx.unlock();
	}
	bool tryReceive() {
		sf::Packet packet;
		if (selector.wait(sf::seconds(1))) {
			if (selector.isReady(socket)) {
				mtx.lock();
				socket.receive(packet);
				mtx.unlock();
				packet >> msgbuf;
				return true;
			}
		}
		return false;
	}
	enum status {
		START,
		CONNECTING,
		GOTIP,
		MAILING,
		ERROR,
		STOP
	} s = START;
};
class letterSenderServer : public letterSender {
	std::string IPnPort;
	std::unique_ptr<std::thread> waiter;

	static void lifeCycle(letterSenderServer* ls) {
//		printf("[0]\n");
		sf::IpAddress address = sf::IpAddress::getLocalAddress();
		sf::TcpListener listener;
		listener.setBlocking(false);
		if (listener.listen(sf::Socket::AnyPort) != sf::Socket::Done) {
			ls->s = ERROR;
			return;
		}
		ls->s = GOTIP;
//		printf("[1]\n");
		int port = listener.getLocalPort();

		ls->IPnPort = address.toString() + "  " + std::to_string(port);
		while (listener.accept(ls->socket) != sf::Socket::Done) {
			if (ls->s == STOP) {
				return;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
//		printf("[2]\n");
		ls->selector.add(ls->socket);

		ls->s = MAILING;
		while (true) {
			if (ls->s == MAILING) {
				ls->tryReceive();
//				printf("[8]\n");
			} else if (ls->s == ERROR || ls->s == STOP) {
				break;
			}
		}
//		printf("[9]\n");
	}
public:
	letterSenderServer() {
		s = CONNECTING;
//		printf("CONNECTING\n");
		waiter = std::make_unique<std::thread>(lifeCycle, this);
	}
	~letterSenderServer() {
		s = STOP;
		waiter->join();
	}
	std::string getIPnPort() const {
		return IPnPort;
	}
};
class letterSenderClient : public letterSender {
	std::unique_ptr<std::thread> waiter;
	static void lifeCycle(letterSenderClient* ls, sf::IpAddress addr, int port) {
		sf::Socket::Status status = ls->socket.connect(addr, port);
		if (status != sf::Socket::Done) {
			ls->s = ERROR;
			return;
		}
		ls->s = MAILING;
		while (true) {
			if (ls->s == MAILING) {
				std::string msg;
				ls->tryReceive();
			} else if (ls->s == ERROR || ls->s == STOP) {
				break;
			}
		}
	}
public:
	letterSenderClient(sf::IpAddress addr, int port) {
		s = CONNECTING;
		waiter = std::make_unique<std::thread>(lifeCycle, this, addr, port);
	}
	~letterSenderClient() {
		s = STOP;
		waiter->join();
	}
};

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
	struct data1_t {
		letterSenderServer ls;
		std::thread th;

		static void lifeCycle(data1_t* data_1, uiText* display) {
			bool w = true;
			while (w) {
//				printf("[3]\n");
				switch (data_1->ls.s) {
					case letterSender::GOTIP: {
//						printf("[4]\n");
						display->setString(data_1->ls.getIPnPort());
						w = false;
					}
					case letterSender::START:
					case letterSender::CONNECTING:break;
					case letterSender::MAILING:
					case letterSender::ERROR:
					case letterSender::STOP: w = false;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
//			printf("[5]\n");
		}
		explicit data1_t(uiText& ipText) :ls() {
			th = std::thread(lifeCycle, this, &ipText);
		}
		~data1_t() {
//			printf("[6]\n");
			ls.s = letterSender::STOP;
			th.join();
//			printf("[7]\n");
		}
	};
	std::unique_ptr<data1_t> data1;

	uiScene welcomeScene(true, {0, 0, 1000, 1000});

	uiText t_ip({50, 850, 950, 950}, scaleMode::scaleXY, "...", "Ninja Naruto.ttf");

	uiScene serverConnectScene(false, {0, 0, 1000, 1000}, [&data1, &t_ip]() {
		data1 = std::make_unique<data1_t>(t_ip);
	});
	uiScene clientConnectScene(false, {0, 0, 1000, 1000});
	uiScene mailingScene(false, {0, 0, 1000, 1000});

	uiText t_info({50, 850, 950, 950}, scaleMode::scaleXY, "select mode:", "Ninja Naruto.ttf");
	uiButton b_serv({50, 400, 450, 800}, scaleMode::scaleXY, {"src0.png"}, {"src1.png"}, uiScene::switchScene(welcomeScene, serverConnectScene));
	uiButton b_client({550, 400, 950, 800}, scaleMode::scaleXY, {"src0.png"}, {"src1.png"}, uiScene::switchScene(welcomeScene, clientConnectScene));
	uiText t_serv({50, 400, 450, 800}, scaleMode::scaleXY, "server", "Ninja Naruto.ttf");
	uiText t_client({550, 400, 950, 800}, scaleMode::scaleXY, "client", "Ninja Naruto.ttf");

	welcomeScene.addUIPart(t_info);
	welcomeScene.addUIPart(b_serv);
	welcomeScene.addUIPart(b_client);
	welcomeScene.addUIPart(t_serv);
	welcomeScene.addUIPart(t_client);

	serverConnectScene.addUIPart(t_ip);

	uiGroup mainGroup(box2::unit(), scaleMode::fullZone, 4);
	mainGroup.setUIPart(0, welcomeScene);
	mainGroup.setUIPart(1, serverConnectScene);
	mainGroup.setUIPart(2, clientConnectScene);
	mainGroup.setUIPart(3, mailingScene);

	window mainWindow("@_@", {1000, 1000});
	mainWindow.setUIScene(mainGroup);
	mainWindow.startRenderCycle();
#endif
#ifdef DEMO3

#endif
	return 0;
}