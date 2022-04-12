#include "chess.h"

int board::tryStep(sf::Vector2i from, sf::Vector2i to) {
	figure moved = map[getField(from.x, from.y)];
	using namespace figures;
	// Проверка корректности перемещения фигуры по ее типу
	switch (MASKF & moved) {
		case ROOK: if (to.x != from.x && to.y != from.y) return 0;
			break;
		case BISHOP: if (to.x + to.y != from.x + from.y && to.x - to.y != from.x - from.y) return 0;
			break;
		case QUEEN:
			if (to.x != from.x && to.y != from.y && to.x + to.y != from.x + from.y
					&& to.x - to.y != from.x - from.y)
				return 0;
			break;
		case KING: if (abs(to.x - from.x) > 1 || abs(to.y - from.y) > 1) return 0;
			break;
		case PAWN:  //omg
			break;
	}
	figure taken = map[getField(from.x, from.y)];

	return -1;
}

void board::playStep(step s) {
	if (!s.type) {
		figure moved = map[s.parts[0].from];
		map[s.parts[0].from] = figures::NONE;
		map[s.parts[1].from] = moved;
		return;
	}
}

std::pair<uint8_t, uint8_t> board::setField(uint8_t field) {
	return {field % 8, field / 8};
}

uint8_t board::getField(uint8_t rank, uint8_t file) {
	uint8_t field = file * 8 + rank;
	return field;
}

board::board(type t) {
	using namespace figures;
	switch (t) {
		case EMPTY: {
			map = std::vector<figure>(64, NONE);
			break;
		}
		case DEFAULT: {
			map = {
					BLACK & ROOK, BLACK & KNIGHT, BLACK & BISHOP, BLACK & QUEEN,
					BLACK & KING, BLACK & BISHOP, BLACK & KNIGHT, BLACK & ROOK,
					BLACK & PAWN, BLACK & PAWN, BLACK & PAWN, BLACK & PAWN,
					BLACK & PAWN, BLACK & PAWN, BLACK & PAWN, BLACK & PAWN,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
					WHITE & PAWN, WHITE & PAWN, WHITE & PAWN, WHITE & PAWN,
					WHITE & PAWN, WHITE & PAWN, WHITE & PAWN, WHITE & PAWN,
					WHITE & ROOK, WHITE & KNIGHT, WHITE & BISHOP, WHITE & QUEEN,
					WHITE & KING, WHITE & BISHOP, WHITE & KNIGHT, WHITE & ROOK
			};
			break;
		}
	}
	flags = 0, timer = 50, story = {}, state = WHITE_TURN;
}
figure& board::at(uint8_t x, uint8_t y) {
	return map[x+y*8];
}
int board::call(int code) {

	return -1;
}
float board::eval() const {
	int sum = 0;
	for(auto f: map){
		sum+=!!f;
	}
	return sum;
}
