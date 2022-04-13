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
		// исправить на что-то вменяемое, если понадобится
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
		void Wait();

		friend class SpriteBase;
		friend class TextSprite;
		friend class ShaderSprite;
		friend class AppManager;
	};

	class AppManager {
	protected:
	public:
		uint32_t currentScene = 0;
		Window* w;

		virtual void OnKeyEvent(uint32_t keyCode);
		virtual void OnSceneSwitch(uint32_t sceneCode);
		virtual void OnBtnClick(uint32_t btnId, sf::Vector2f pos);
		virtual void OnExit();
		void Unblock();

		void SwitchScene(uint32_t sceneId);
		explicit AppManager(Window& w);
	};
	class AppManagerDefault : public AppManager {
		typedef std::function<void(uint32_t, sf::Vector2f, AppManager*)> obc_t;
		typedef std::function<void(uint32_t, AppManager*)> oke_t, oss_t;
		typedef std::function<void(AppManager*)> oe_t;

		void OnKeyEvent(uint32_t keyCode) override;
		void OnSceneSwitch(uint32_t sceneCode) override;
		void OnBtnClick(uint32_t id, sf::Vector2f pos) override;
		void OnExit() override;
	public:
		obc_t obc = [](uint32_t, sf::Vector2f, AppManager* am) { am->Unblock(); };
		oke_t oke = [](uint32_t, AppManager*) { };
		oss_t oss = [](uint32_t, AppManager*) { };
		oe_t oe = [](AppManager* am) { am->AppManager::OnExit(); };

		explicit AppManagerDefault(Window& w);
	};
}
