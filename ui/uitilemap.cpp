#include "uielements.h"

namespace ui {
	Box2 TileMap::SubBox(sf::Vector2i i) {
		return boxScaled * Box2(sf::Vector2f(i) / sf::Vector2f(gridSize), (sf::Vector2f(i) + sf::Vector2f(1, 1)) / sf::Vector2f(gridSize));
	}
	void TileMap::SetIndexes(std::vector<uint32_t> _map, sf::Vector2u _gridSize) {
		if (_gridSize != sf::Vector2u(-1, -1)) {
			gridSize = _gridSize;
		}
		assert(gridSize.x * gridSize.y == _map.size());
		map = std::move(_map);
		fresh = false;
	}
	void TileMap::SetByIndex(uint32_t cellId, uint32_t texId) {
		map[cellId] = texId;
		fresh = false;
	}
	sf::Vector2i TileMap::Proj(sf::Vector2f pos) {
		pos = ToUnit(pos);
		return {(int)(pos.x * (float)gridSize.x), (int)(pos.y * (float)gridSize.y)};
	}
	size_t TileMap::Size() const {
		return gridSize.x * gridSize.y;
	}

	void TileMap::Draw(Window* w) {
		if (visible) {
			sf::Vector2i i;
			int j = 0;
			for (i.y = 0; i.y < gridSize.y; i.y++) {
				for (i.x = 0; i.x < gridSize.x; i.x++, j++) {
					tilemap[map[j]].Draw(w, SubBox(i));
				}
			}
		}
	}
	TileMap::TileMap(Box2 zone, ScaleMode sm, const std::string& src, sf::Vector2u srcGridSize, uint32_t beg, uint32_t end) :Element(zone, sm) {
		visible = true;
		map = {};
		gridSize = {};
		if (end == -1) end = srcGridSize.x * srcGridSize.y;
		tilemap.assign(end - beg, {});
		for (uint32_t i = 0, j = beg; j < end; i++, j++) {
			tilemap[i] = Sprite({src, srcGridSize, j});
		}
	}

	sf::Vector2i SelectionTM::GetSelPos() const {
		return selPos;
	}
	void SelectionTM::Select(sf::Vector2i pos) {
		selPos = pos;
		fresh = false;
	}
	void SelectionTM::Click(sf::Vector2i pos) {
		selPos = (selPos == pos) ? sf::Vector2i(-1, -1) : pos;
		fresh = false;
	}

	SelectionTM::SelectionTM(TileMap* tm, const Sprite::Param& src) :Element(tm->boxOrigin, tm->sm), spr(src) {
		this->tm = tm;
		selPos = {-1, -1};
		visible = true;
	}
	void SelectionTM::Draw(Window* w) {
		if (selPos != sf::Vector2i(-1, -1)) {
			spr.Draw(w, tm->SubBox(selPos));
		}
	}
}
