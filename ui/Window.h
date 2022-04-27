#pragma once
#include <map>
#include <vector>
#include "../specials/Box2.h"
#include <SFML/Graphics.hpp>
#include "uielements.h"
#include <functional>
#include <cassert>
#include <thread>

namespace ui {
	class Element;
	class AppManager;

	class Window {
		enum state_t {
			PREPARE,
			BLOCKED,
			RUN,
			STOP,
			ERROR
		} state;

		std::thread renderThread;
		std::vector<Element*> scenes;
		std::vector<Element*> ordered;
		uint32_t currentScene = 0;
		AppManager* manager = nullptr;

		sf::RenderWindow rw;
		sf::Vector2f sizeOrigin, sizeScaled;
		std::string name;
	public:
		void Refresh(bool check);

		void RenderCycle();
		void StartRenderCycle();

		void SetScene(Element* uiel, uint32_t id);
		void SetManager(AppManager& manager);
		void SwitchScene(uint32_t sceneId);

		Window(const std::string& name, sf::Vector2f size, uint32_t sceneCount);
		~Window();

		friend class Sprite;
		friend class TextSprite;
		friend class ShaderSprite;
		friend class AppManager;
	};

	class AppManager {
	protected:
	public:
		uint32_t currentScene = 0;
		Window* w;

		void OnKeyEvent(uint32_t keyCode);
		void OnSceneSwitch(uint32_t sceneCode);
		void OnBtnClick(uint32_t btnId, sf::Vector2f pos);
		void OnExit();
		void Close() const;
		void Unblock() const;

		typedef std::function<void(AppManager*, uint32_t, sf::Vector2f)> obc_t;
		typedef std::function<void(AppManager*, uint32_t)> oke_t, oss_t;
		typedef std::function<void(AppManager*)> oe_t;

		obc_t obc = [](AppManager* self, uint32_t, sf::Vector2f) { self->Unblock(); };
		oke_t oke = [](AppManager* self, uint32_t) { };
		oss_t oss = [](AppManager* self, uint32_t) { };
		oe_t oe = [](AppManager* self) { self->Close(); };

		void SwitchScene(uint32_t sceneId);
		explicit AppManager(Window& w);
	};
}
