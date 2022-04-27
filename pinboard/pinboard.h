#pragma once
#include <cstdint>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>

typedef uint8_t Pin;
namespace pins {
	const Pin NONE = 0, A = 1, B = 2, C = 3, D = 4, E = 5, F = 6, MAX = 7;
}

class Pinboard {
public:
	enum State {
		PLAYER,
		BOT,
		END
	};
private:
	uint32_t size;
	std::vector<Pin> data;
	State state;

	Pin& at(uint32_t x, uint32_t y) {
		return data[size * y + x];
	}

	std::thread botThread;
	void BotCycle() {
		while (state != END) {
			if (state == BOT) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				at(rand() % size, rand() % size) = rand() % pins::MAX;
				mtx.lock();
				if (state != END) {
					state = PLAYER;
				}
				mtx.unlock();
			}
		}
	}
public:
	explicit Pinboard(uint32_t size) {
		this->size = size;
		data = std::vector<Pin>(size * size, pins::NONE);
		state = PLAYER;
		botThread = std::thread(&Pinboard::BotCycle, this);
	}
	~Pinboard() {
		SetState(END);
		botThread.join();
	}
	int step(uint32_t x, uint32_t y, Pin pin) {
		switch (state) {
			case PLAYER: {
				at(x, y) = pin;
				SetState(BOT);
				return 1;
			}
			case BOT: {
				std::cout << "not your turn!\n";
				break;
			}
			case END: {
				std::cout << "how?\n";
			}
		}
		return 0;
	}
	const std::vector<Pin>& GetData() const {
		return data;
	}

	std::mutex mtx;
	void SetState(State s) {
		mtx.lock();
		state = s;
		mtx.unlock();
	}
	State GetState() const {
		return state;
	}
};