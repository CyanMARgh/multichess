#include "chess.h"
#include <vector>
#include <bitset>
#include <algorithm>

std::vector<sf::Vector2i> Board::PossibleMoves(const uint8_t& x, const uint8_t& y) {
	Figure figure = map[GetField(x, y)];
	std::vector<sf::Vector2i> ans;
	using namespace figures;
	switch (MASKF & figure) {
		case NONE: break;
		case ROOK:
			for (int i = x - 1; i >= 0; i--) {
				if ((MASKF & map[GetField(i, y)]) == NONE) ans.emplace_back(sf::Vector2i(i, y));
				else {
					if ((MASKC & map[GetField(i, y)]) != (MASKC & figure)) {
						ans.emplace_back(i, y);
					}
					i -= 8;
				}
			}
			for (int i = x + 1; i <= 7; i++) {
				if ((MASKF & map[GetField(i, y)]) == NONE) ans.emplace_back(sf::Vector2i(i, y));
				else {
					if ((MASKC & map[GetField(i, y)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(i, y));
					}
					i += 8;
				}
			}
			for (int i = y - 1; i >= 0; i--) {
				if ((MASKF & map[GetField(x, i)]) == NONE) ans.emplace_back(sf::Vector2i(x, i));
				else {
					if ((MASKC & map[GetField(x, i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x, i));
					}
					i -= 8;
				}
			}
			for (int i = y + 1; i <= 7; i++) {
				if ((MASKF & map[GetField(x, i)]) == NONE) ans.emplace_back(sf::Vector2i(x, i));
				else {
					if ((MASKC & map[GetField(x, i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x, i));
					}
					i += 8;
				}
			}
			break;
		case BISHOP:
			for (int i = 1; i <= 7; i++) {
				if (y - i < 0 || x - i < 0) continue;
				if ((MASKF & map[GetField(x - i, y - i)]) == NONE) ans.emplace_back(sf::Vector2i(x - i, y - i));
				else {
					if ((MASKC & map[GetField(x - i, y - i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x - i, y - i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x - i < 0 || y + i > 7) continue;
				if ((MASKF & map[GetField(x - i, y + i)]) == NONE) ans.emplace_back(sf::Vector2i(x - i, y + i));
				else {
					if ((MASKC & map[GetField(x - i, y + i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x - i, y + i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x + i > 7 || y - i < 0) continue;
				if ((MASKF & map[GetField(x + i, y - i)]) == NONE) ans.emplace_back(sf::Vector2i(x + i, y - i));
				else {
					if ((MASKC & map[GetField(x + i, y - i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x + i, y - i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x + i > 7 || y + i > 7) continue;
				if ((MASKF & map[GetField(x + i, y + i)]) == NONE) ans.emplace_back(sf::Vector2i(x + i, y + i));
				else {
					if ((MASKC & map[GetField(x + i, y + i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x + i, y + i));
					}
					i += 8;
				}
			}
			break;
		case QUEEN:
			for (int i = x - 1; i >= 0; i--) {
				if ((MASKF & map[GetField(i, y)]) == NONE) ans.emplace_back(sf::Vector2i(i, y));
				else {
					if ((MASKC & map[GetField(i, y)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(i, y));
					}
					i -= 8;
				}
			}
			for (int i = x + 1; i <= 7; i++) {
				if ((MASKF & map[GetField(i, y)]) == NONE) ans.emplace_back(sf::Vector2i(i, y));
				else {
					if ((MASKC & map[GetField(i, y)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(i, y));
					}
					i += 8;
				}
			}
			for (int i = y - 1; i >= 0; i--) {
				if ((MASKF & map[GetField(x, i)]) == NONE) ans.emplace_back(sf::Vector2i(x, i));
				else {
					if ((MASKC & map[GetField(x, i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x, i));
					}
					i -= 8;
				}
			}
			for (int i = y + 1; i <= 7; i++) {
				if ((MASKF & map[GetField(x, i)]) == NONE) ans.emplace_back(sf::Vector2i(x, i));
				else {
					if ((MASKC & map[GetField(x, i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x, i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (y - i < 0 || x - i < 0) continue;
				if ((MASKF & map[GetField(x - i, y - i)]) == NONE) ans.emplace_back(sf::Vector2i(x - i, y - i));
				else {
					if ((MASKC & map[GetField(x - i, y - i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x - i, y - i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x - i < 0 || y + i > 7) continue;
				if ((MASKF & map[GetField(x - i, y + i)]) == NONE) ans.emplace_back(sf::Vector2i(x - i, y + i));
				else {
					if ((MASKC & map[GetField(x - i, y + i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x - i, y + i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x + i > 7 || y - i < 0) continue;
				if ((MASKF & map[GetField(x + i, y - i)]) == NONE) ans.emplace_back(sf::Vector2i(x + i, y - i));
				else {
					if ((MASKC & map[GetField(x + i, y - i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x + i, y - i));
					}
					i += 8;
				}
			}
			for (int i = 1; i <= 7; i++) {
				if (x + i > 7 || y + i > 7) continue;
				if ((MASKF & map[GetField(x + i, y + i)]) == NONE) ans.emplace_back(sf::Vector2i(x + i, y + i));
				else {
					if ((MASKC & map[GetField(x + i, y + i)]) != (MASKC & figure)) {
						ans.emplace_back(sf::Vector2i(x + i, y + i));
					}
					i += 8;
				}
			}
			break;
		case KNIGHT: if (x - 2 >= 0 && y + 1 <= 7 && (MASKC & map[GetField(x - 2, y + 1)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x - 2, y + 1));
			if (x - 2 >= 0 && y - 1 >= 0 && (MASKC & map[GetField(x - 2, y - 1)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x - 2, y - 1));
			if (x - 1 >= 0 && y + 2 <= 7 && (MASKC & map[GetField(x - 1, y + 2)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x - 1, y + 2));
			if (x - 1 >= 0 && y - 2 >= 0 && (MASKC & map[GetField(x - 1, y - 2)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x - 1, y - 2));
			if (x + 1 <= 7 && y + 2 <= 7 && (MASKC & map[GetField(x + 1, y + 2)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x + 1, y + 2));
			if (x + 1 <= 7 && y - 2 >= 0 && (MASKC & map[GetField(x + 1, y - 2)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x + 1, y - 2));
			if (x + 2 <= 7 && y + 1 <= 7 && (MASKC & map[GetField(x + 2, y + 1)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x + 2, y + 1));
			if (x + 2 <= 7 && y - 1 >= 0 && (MASKC & map[GetField(x + 2, y - 1)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x + 2, y - 1));
			break;
		case KING:
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (x + i >= 0 && x + i <= 7 && y + j >= 0 && y + j <= 7) {
						if ((MASKC & map[GetField(x + i, y + j)]) != (MASKC & figure)) ans.emplace_back(sf::Vector2i(x + i, y + j));
					}
				}
			}
			break;
		case PAWN:
			if ((MASKC & figure) == WHITE) {
				if ((MASKF & map[GetField(x, y + 1)]) == NONE) ans.emplace_back(sf::Vector2i(x, y + 1));
				if (y == 1 && (MASKF & map[GetField(x, y + 2)]) == NONE && (MASKF & map[GetField(x, y + 2)]) == NONE)
					ans.emplace_back(sf::Vector2i(x, y + 2)); // на 2 со стартовой позиции
				if (x - 1 >= 0) {
					if ((MASKC & map[GetField(x - 1, y + 1)]) == BLACK) ans.emplace_back(sf::Vector2i(x - 1, y + 1));
				}
				if (x + 1 <= 7) {
					if ((MASKC & map[GetField(x + 1, y + 1)]) == BLACK) ans.emplace_back(sf::Vector2i(x + 1, y + 1));
				}
			}
			if ((MASKC & figure) == BLACK) {
				if ((MASKF & map[GetField(x, y - 1)]) == NONE) ans.emplace_back(sf::Vector2i(x, y - 1));
				if (y == 6 && (MASKF & map[GetField(x, y - 2)]) == NONE && (MASKF & map[GetField(x, y - 2)]) == NONE)
					ans.emplace_back(sf::Vector2i(x, y - 2)); // на 2 со стартовой позиции
				if (x - 1 >= 0) {
					if ((MASKC & map[GetField(x - 1, y - 1)]) == WHITE) ans.emplace_back(sf::Vector2i(x - 1, y - 1));
				}
				if (x + 1 <= 7) {
					if ((MASKC & map[GetField(x + 1, y - 1)]) == WHITE) ans.emplace_back(sf::Vector2i(x + 1, y - 1));
				}
			}
			break;
	}
	return ans;
}

std::vector<sf::Vector2i> Board::PossibleCastlings(const uint8_t& x, const uint8_t& y) {
	// Рокировки
	Figure figure = map[GetField(x, y)];
	std::vector<sf::Vector2i> ans;
	using namespace figures;
	switch (MASKF & figure) {
		case KING:
			if ((MASKC & figure) == WHITE && white_king) {
				std::set<std::pair<int, int>> attacked = GetAttackedFields();
				// короткая рокировка
				if (attacked.find({4, 0}) == attacked.end() && attacked.find({5, 0}) == attacked.end() && \
                attacked.find({6, 0}) == attacked.end() && (MASKF & map[GetField(5, 0)]) == NONE && \
                (MASKF & map[GetField(6, 0)]) == NONE) {
					ans.emplace_back(sf::Vector2i(6, 0));
				}
				// длинная рокировка
				if (attacked.find({2, 0}) == attacked.end() && \
                attacked.find({3, 0}) == attacked.end() && attacked.find({4, 0}) == attacked.end() && \
                (MASKF & map[GetField(3, 0)]) == NONE && \
                (MASKF & map[GetField(2, 0)]) == NONE && \
                (MASKF & map[GetField(1, 0)]) == NONE) {
					ans.emplace_back(sf::Vector2i(2, 0));
				}
			}
	}
	return ans;
}

std::vector<sf::Vector2i> Board::PossibleTOA(const uint8_t& x, const uint8_t& y) {
	// взятия на проходе
	Figure figure = map[GetField(x, y)];
	std::vector<sf::Vector2i> ans;
	if ((figures::MASKF & figure) != figures::PAWN) return ans;
	if ((figures::MASKC & figure) == figures::WHITE && y == 4) {
		if (story.back().parts[0].from.y == 6 && story.back().parts[0].from.x == x + 1) {
			ans.emplace_back(sf::Vector2i(x + 1, y + 1));
		}
		if (story.back().parts[0].from.y == 6 && story.back().parts[0].from.x == x - 1) {
			ans.emplace_back(sf::Vector2i(x - 1, y + 1));
		}
	}
	if ((figures::MASKC & figure) == figures::BLACK && y == 3) {
		if (story.back().parts[0].from.y == 1 && story.back().parts[0].from.x == x + 1) {
			ans.emplace_back(sf::Vector2i(x + 1, y - 1));
		}
		if (story.back().parts[0].from.y == 1 && story.back().parts[0].from.x == x - 1) {
			ans.emplace_back(sf::Vector2i(x - 1, y - 1));
		}
	}
	return ans;
}

std::set<std::pair<int, int>> Board::GetAttackedFields() {
	std::set<std::pair<int, int>> attacked;
	Figure side;
	if (state == WHITE_TURN) side = figures::WHITE;
	if (state == BLACK_TURN) side = figures::BLACK;
	for (int x = 0; x <= 7; x++) {
		for (int y = 0; y <= 7; y++) {
			if ((figures::MASKC & map[GetField(x, y)]) == side)
				for (auto pos : PossibleMoves(x, y)) {
					attacked.insert({pos.x, pos.y});
				}
		}
	}
	return attacked;
}

void Board::PlayStep(Step s) {
	if (state == WHITE_TURN) state = BLACK_TURN;
	else if (state == BLACK_TURN) state = WHITE_TURN;
	// обычный ход
	if (!s.type) {
		Figure moved = map[GetField(s.parts[0].from.x, s.parts[0].from.y)];
		map[GetField(s.parts[0].from.x, s.parts[0].from.y)] = figures::NONE;
		map[GetField(s.parts[0].to.x, s.parts[0].to.y)] = moved;
		return;
	}
}

void Board::UndoStep() {
	Step s = story.back();
	if (state == WHITE_TURN) state = BLACK_TURN;
	else if (state == BLACK_TURN) state = WHITE_TURN;
	if (!s.type) {
		Figure moved = map[GetField(s.parts[0].to.x, s.parts[0].to.y)];
		map[GetField(s.parts[0].from.x, s.parts[0].from.y)] = moved;
		map[GetField(s.parts[0].to.x, s.parts[0].to.y)] = s.parts[0].f;
		return;
	}
};

std::pair<uint8_t, uint8_t> Board::SetField(uint8_t field) {
	return {field % 8, field / 8};
}

uint8_t Board::GetField(uint8_t rank, uint8_t file) {
	return file * 8 + rank;
}

Figure Board::GetState() {
	if (state == WHITE_TURN) return figures::WHITE;
	return figures::BLACK;
}

Board::Board(type t) {
	using namespace figures;
	switch (t) {
		case EMPTY: {
			map = std::vector<Figure>(64, NONE);
			break;
		}
		case DEFAULT: {
			map = std::vector<Figure>{
					WHITE | ROOK, WHITE | KNIGHT, WHITE | BISHOP, WHITE | QUEEN,
					WHITE | KING, WHITE | BISHOP, WHITE | KNIGHT, WHITE | ROOK,
					WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
					WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
					BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
					BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN,
					BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK
			};
			break;
		}
	}
	white_king = true, black_king = true, timer = 50, story = {}, state = WHITE_TURN;
}

Figure& Board::At(uint8_t x, uint8_t y) {
	return map[x + y * 8];
}
Figure Board::At(uint8_t x, uint8_t y) const {
	return map[x + y * 8];
}

bool Board::PlayerStep(sf::Vector2i from, sf::Vector2i to) {
	if ((figures::MASKC & At(from.x, from.y)) != GetState()) {
		return false;
	} else {
		std::vector<sf::Vector2i> possible = PossibleMoves(from.x, from.y);
		if (std::find(possible.begin(), possible.end(), to) != possible.end()) {
			// предварительно, ход возможен
			// сделаем его
			bool done = true;
			Step s;
			s.type = 0;
			s.parts[0].from = from;
			s.parts[0].to = to;
			s.parts[0].f = At(to.x, to.y);
			PlayStep(s);
			// и посмотрим, не подставлен ли под бой король
			std::set<std::pair<int, int>> attacked = GetAttackedFields();
			{
				using namespace figures;
				for (int i = 0; i <= 7; i++) {
					for (int j = 0; j <= 7; j++) {
						if ((MASKF & At(i, j)) == KING &&
								(MASKC & At(i, j)) != GetState() &&
								attacked.find({i, j}) != attacked.end()) {
							UndoStep();
							done = false;
						}
					}
				}
			}
			if (done) return true;
		} else {
			return false;
		}
	}
	return false;
}

void Board::Reset(){
	white_king = true;
	black_king = true;
	story.clear();
	state = WHITE_TURN;
	using namespace figures;
	map = std::vector<Figure>{
			WHITE | ROOK, WHITE | KNIGHT, WHITE | BISHOP, WHITE | QUEEN,
			WHITE | KING, WHITE | BISHOP, WHITE | KNIGHT, WHITE | ROOK,
			WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
			WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
			NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
			NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
			NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
			NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
			BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
			BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
			BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN,
			BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK
	};
}

