#pragma once
#include "../specials/Box2.h"
#include "Window.h"
#include "cstring"
#include <functional>
#include <cstdint>
#include <unordered_set>
#include <memory>
#include <utility>

namespace ui {
	class Window;
	class Sprite;
	struct SpriteParam {
		std::string s;
		sf::Vector2u gm = {1, 1};
		uint32_t i = 0;
		enum Type {
			NONE,
			SINGLE,
			SCALE9
		} type = SINGLE;
		float size0 = 0.f;
	};
	struct SpriteBase {
		std::string name;
		sf::Texture tex;
		sf::Sprite spr;
		sf::Vector2u s0;
		SpriteBase(const sf::Image& img, const std::string& name, sf::IntRect rect);
		SpriteBase& operator=(const SpriteBase&) = delete;
		void Draw(Window* w, Box2 zone);
		static std::unordered_set<std::shared_ptr<SpriteBase>> loadedSprites;
	};
	class Sprite {
		std::vector<std::shared_ptr<SpriteBase>> sbptrs;
		SpriteParam::Type type;
	public:
		static void LoadSpriteSheet(const std::string& src, sf::Vector2u gridMetrics);
		Sprite();
		explicit Sprite(const SpriteParam& par);
		void Draw(Window* w, Box2 zone);
	};
	class TextSprite {
		sf::Font font;
		sf::Text text;
		std::string stext;
	public:
		TextSprite(const std::string& fontSrc, const std::string& textSrc);
		void SetText(const std::string& textSrc);
		void Draw(Window* w, Box2 zone);
	};
	class ShaderSprite {
		sf::RectangleShape rs;
		sf::Shader sh;
		float time;
	public:
		explicit ShaderSprite(const std::string& src);
		void SetTime(float time);
		void Draw(Window* w, Box2 zone);
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
		uint32_t flags = 0b00000000'00000000'00000000'00000001;

		enum Flag : uint32_t {
			VISIBLE = 1 << 0,
			CLICKABLE = 1 << 1,
			PRESSED = 1 << 2,
			FRESH = 1 << 3
		};
		bool Is(Flag flag) const;
		void Set(Flag flag, bool value);

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
		std::vector<std::unique_ptr<Element>> parts;
		int pressedPart = -1;

		void Reshape(Box2 parentBoxOrigin, Box2 parentBoxScaled) final;
	protected:
		void Draw(Window* w) override;
		int OnMouseEvent(MouseEvent event, sf::Vector2f pos) override;
		virtual Box2 getSubBox(uint i);
	public:
		void AddPartsOrdered(std::vector<Element*>& ordered) override;
		explicit Group(Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone, uint count = 0);
		void AddUIPart(Element* uiel);
		void SetPart(Element* uiel, uint32_t id);
	};

	class Image final : public Element {
	protected:
		Sprite spr;
		void Draw(Window* w) override;
	public:
		Image(Box2 zone, ScaleMode sm, const SpriteParam& src);
	};
	class Text final : public Element {
	protected:
		TextSprite spr;
		void Draw(Window* w) override;
	public:
		Text(Box2 zone, ScaleMode sm, const std::string& textSrc, const std::string& fontSrc);
		void SetString(const std::string& s);
	};
	class Shader final : public Element {
	protected:
		ShaderSprite spr;
		void Draw(Window* w) final;
	public:
		Shader(Box2 zone, ScaleMode sm, const std::string& src);
		void SetTime(float time);
	};

	class SelectionTM;
	class TileMap final : public Element {
		std::vector<uint32_t> map;
		sf::Vector2u gridSize;
		std::vector<Sprite> tilemap;

		void Draw(Window* w) final;
		size_t Size() const;
	public:
		TileMap(Box2 zone, ScaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg = 0, uint32_t end = -1);

		void SetIndexes(std::vector<uint32_t> map, sf::Vector2u _gridSize);
		void SetByIndex(uint32_t cellId, uint32_t texId);
		Box2 SubBox(sf::Vector2i i);
		sf::Vector2i Proj(sf::Vector2f pos);

		friend class SelectionTM;
	};
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
		Button(Box2 zone, ScaleMode sm, const SpriteParam& parFree, const SpriteParam& parPressed, uint32_t id);
	};
	class SelectionTM : public Element {
		TileMap* tm;
		Sprite spr;
		sf::Vector2i selPos;

		void Draw(Window* w) override;
	public:
		SelectionTM(TileMap* tm, const SpriteParam& src);

		sf::Vector2i getSelPos() const;
		void Select(sf::Vector2i pos);
		void Click(sf::Vector2i pos);
	};

	class SideCut : public Group {
		Box2 getSubBox(uint i) final;
		sf::Vector2f subSize;
	public:
		explicit SideCut(sf::Vector2f subSize, Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone);
	};
	class Grid : public Group {
		Box2 getSubBox(uint i) final;
		sf::Vector2u metrics;
	public:
		explicit Grid(sf::Vector2u metrics, Box2 zone = Box2::Unit(), ScaleMode sm = ScaleMode::fullZone);
	};
}

