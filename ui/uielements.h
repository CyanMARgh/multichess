#pragma once
#include "../utils/Box2.h"
#include "cstring"
#include <functional>
#include <cstdint>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <cassert>
#include <thread>

namespace ui {
	class Window;
	class Sprite;
	class Sprite {
	public:
		struct Param {
			std::string s;
			sf::Vector2u gm = {1, 1};
			uint32_t beg = 0;
			enum Type {
				NONE,
				SINGLE,
				SCALE9
			} type = SINGLE;
			sf::Vector2f size0 = {0.f, 0.f};

			static Type GetType(const std::string& s);
		};
	private:
		struct Base {
			std::string name;
			sf::Texture tex;
			sf::Sprite spr;
			sf::Vector2u s0;
			Base(const sf::Image& img, const std::string& name, sf::IntRect rect);
			Base& operator=(const Base&) = delete;
			void Draw(Window* w, Box2 zone);
		};
		static std::unordered_set<std::shared_ptr<Base>> loadedSprites;

		std::vector<std::shared_ptr<Base>> sbptrs;
		Param params;
	public:
		static void LoadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics);
		Sprite();
		explicit Sprite(const Param& par);
		void Draw(Window* w, Box2 zone);

		friend class Window;
	};
	class TextSprite {
		sf::Font font;
		sf::Text text;
		std::string stext;
	public:
		TextSprite(const std::string& fontSrc, const std::string& textSrc);
		void SetText(const std::string& textSrc);
		void Draw(Window* w, Box2 zone, uint32_t color);
	};
	class ShaderSprite {
		sf::RectangleShape rs;
		sf::Shader sh;
		std::unordered_map<std::string, float> uniformsFloat{};
		std::unordered_map<std::string, std::shared_ptr<sf::Texture>> uniformsTex{};
		std::unordered_map<std::string, sf::Vector2f> uniformsVec2{};
	public:
		explicit ShaderSprite(const std::string& src);

		void SetUniform(const std::string& name, float val) {
			uniformsFloat[name] = val;
		}
		void SetUniform(const std::string& name, sf::Vector2f val) {
			uniformsVec2[name] = val;
		}
		void SetUniform(const std::string& name, const std::string& src) {
			std::shared_ptr<sf::Texture> tex = nullptr;
			auto found = uniformsTex.find(name);
			if(found == uniformsTex.end()) {
				tex = std::make_shared<sf::Texture>();
				tex->loadFromFile(src);
				uniformsTex.insert({name, tex});
			} else {
				tex = found->second;
			}
		}

		void Draw(Window* w, Box2 zone);

		ShaderSprite(const ShaderSprite&) = delete;
		ShaderSprite& operator=(const ShaderSprite&) = delete;
	};

	enum class ScaleMode {
		bindBL,
		bindTR,
		bindTL,
		bindBR,
		scaleXY,
		fitCenter,
		fullZone
	};
	enum class MouseEvent {
		pressing,
		holding,
		release
	};

	class Element {
	protected:
		ScaleMode sm;
		Box2 boxOrigin, boxScaled;

		Element(Box2 zone, ScaleMode sm);
	public:
		bool visible : 1;
		bool clickable : 1;
		bool pressed : 1;
		bool fresh : 1;

		sf::Vector2f ToUnit(sf::Vector2f pos) const;

		virtual void AddPartsOrdered(std::vector<Element*>& ordered);
		bool IsInside(sf::Vector2f pos) const;
		virtual void Reshape(Box2 parentBoxOrigin, Box2 parentBoxScaled);
		void Reshape(sf::Vector2f parentSizeOrigin, sf::Vector2f parentBoxScaled);

		virtual void Draw(Window* w);
		const int NO_HIT = -2, HIT_NO_ACTION = -1;
		virtual int OnMouseEvent(MouseEvent event, sf::Vector2f pos);

		virtual ~Element() = default;
	};
	class Group : public Element {
		int pressedPart = -1;

		void Reshape(Box2 parentBoxOrigin, Box2 parentBoxScaled) final;
	protected:
		std::vector<std::unique_ptr<Element>> parts;
		void Draw(Window* w) override;
		int OnMouseEvent(MouseEvent event, sf::Vector2f pos) override;
		virtual Box2 GetSubBox(uint i);
	public:
		void AddPartsOrdered(std::vector<Element*>& ordered) final;
		explicit Group(Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone, uint count = 0);
		void AddUIPart(Element* uiel);
		void SetPart(Element* uiel, uint32_t id);
	};

	class Image final : public Element {
	protected:
		Sprite spr;
		void Draw(Window* w) override;
	public:
		Image(Box2 zone, ScaleMode sm, const Sprite::Param& src);
	};
	class Text final : public Element {
	protected:
		TextSprite spr;
		void Draw(Window* w) override;
		uint32_t color;
	public:
		Text(Box2 zone, ScaleMode sm, const std::string& textSrc, const std::string& fontSrc, uint32_t color = ~0);
		void SetString(const std::string& s);
	};
	class Shader final : public Element {
	protected:
		ShaderSprite spr;
		void Draw(Window* w) final;
	public:
		Shader(Box2 zone, ScaleMode sm, const std::string& src);
		std::mutex uniform_mtx;
		template<typename T>
		void SetUniform(const std::string& name, T arg) {
			uniform_mtx.lock();
			fresh = false;
			spr.SetUniform(name, arg);
			uniform_mtx.unlock();
		}
	};

	class SelectionTM;
	class InvisibleButton : public Element {
		uint32_t id;
	protected:
		int OnMouseEvent(MouseEvent event, sf::Vector2f pos) override;
	public:
		InvisibleButton(Box2 zone, ScaleMode sm, uint32_t id);
	};
	class Button final : public InvisibleButton {
		Sprite sprf, sprp;
	protected:
		void Draw(Window* w) override;
	public:
		Button(Box2 zone, ScaleMode sm, const Sprite::Param& parFree, const Sprite::Param& parPressed, uint32_t id);
	};

	class TileMap final : public Element {
		std::vector<uint32_t> map;
		sf::Vector2u gridSize;
		std::vector<Sprite> tilemap;

		void Draw(Window* w) final;
		size_t Size() const;
	public:
		TileMap(Box2 zone, ScaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg = 0, uint32_t end = -1);

		void SetIndexes(std::vector<uint32_t> map, sf::Vector2u _gridSize = sf::Vector2u(-1, -1));
		void SetByIndex(uint32_t cellId, uint32_t texId);
		Box2 SubBox(sf::Vector2i i);
		sf::Vector2i Proj(sf::Vector2f pos);

		friend class SelectionTM;
	};
	class SelectionTM : public Element {
		TileMap* tm;
		Sprite spr;
		sf::Vector2i selPos;

		void Draw(Window* w) override;
	public:
		SelectionTM(TileMap* tm, const Sprite::Param& src);

		sf::Vector2i GetSelPos() const;
		void Select(sf::Vector2i pos);
		void Click(sf::Vector2i pos);
	};

	class SideCut : public Group {
		Box2 GetSubBox(uint i) final;
		sf::Vector2f subSize;
	public:
		explicit SideCut(sf::Vector2f subSize, Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone);
	};
	class Grid : public Group {
		Box2 GetSubBox(uint i) final;
		sf::Vector2u metrics;
	public:
		explicit Grid(sf::Vector2u metrics, Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone);
	};
	class Variant final : public Group {
		uint32_t selected;
		Box2 GetSubBox(uint i) final;
	public:
		explicit Variant(Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone);

		void SwitchTo(uint32_t id);

		void InitVisibility();
		void Draw(Window* w) final;
	};

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

		sf::Vector2f sizeOrigin, sizeScaled;
		std::string name;
		sf::RenderWindow rw;

		sf::Vector2f mousePos;
		void RefreshMousePos();
	public:

		sf::Vector2f GetMousePos() const;
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
		AppManager(const AppManager&) = delete;
		const AppManager& operator=(const AppManager&) = delete;
		~AppManager();
	};
}

