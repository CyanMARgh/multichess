#include "../ui/uielements.h"

namespace ui {
	void AppManager::OnKeyEvent(uint32_t keyCode) {
		oke(this, keyCode);
	}
	void AppManager::OnSceneSwitch(uint32_t sceneCode) {
		oss(this, sceneCode);
	}
	void AppManager::OnExit() {
		oe(this);
	}
	void AppManager::OnBtnClick(uint32_t btnId, sf::Vector2f pos) {
		obc(this, btnId, pos);
		Unblock();
	}

	void AppManager::SwitchScene(uint32_t sceneId) {
		currentScene = sceneId;
		w->SwitchScene(sceneId);
	}
	void AppManager::Unblock() const {
		if (w->state != Window::STOP && w->state != Window::ERROR) {
			w->state = Window::RUN;
		}
	}
	void AppManager::Close() const {
		w->state = Window::STOP;
	}
	AppManager::AppManager(Window& w) : currentScene(0), w(&w) {
		w.SetManager(*this);
	}
	AppManager::~AppManager() {
		while (w->state != Window::STOP && w->state != Window::ERROR) {
//			printf("-");
		}
	}

}