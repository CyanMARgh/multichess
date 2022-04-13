#pragma once
#include "SFML/System/Vector2.hpp"
#include "cstdint"
#include <vector>
#include <list>
#include <string>

// namespace вместо enum-а, чтобы можно было проще проводить побитовые операции
typedef uint8_t figure;
namespace figures {
	const figure
			MASKC = 0b111'00000,
			MASKF = 0b000'11111,
			NONE = 0b000'00000,

			PAWN = 0b000'00001,
			BISHOP = 0b000'00010,
			ROOK = 0b000'00011,
			KNIGHT = 0b000'00100,
			QUEEN = 0b000'00101,
			KING = 0b000'00110,

			WHITE = 0b001'00000,
			BLACK = 0b010'00000;
}

struct step {
	bool side; // Кто ходит (черные/белые)
	bool type; // Обычный ход - 0, особый (рокировка / взятие на проходе / превращение) - 1
	struct part {
//      sf::Vector2i from, to;
		uint8_t from, to;
		figure f;
	}; // part ~ передвинуть фигуру из точки from в точку to (и поставить поверх фигуры f)

//  struct part {   // альтернативный вариант ~ заменить фигуру на at с bef на aft. не уверен, какой вариант лучше.
//		sf::Vector2i at;
//		figure bef, aft;
//	};
	part parts[2]; // в первом варианте хватит двух, во втором нужно до 4х, но второй вариант может быть более гибким.
	uint16_t timeBef; // значение таймера перед ходом.
	uint16_t flagsBef; // значение флагов перед ходом.

	std::string asNotation() const; // привести к виду шахматной нотации
};

class board {
	sf::Vector2i size = {8, 8};
	std::vector<figure> map; //массив фигур
	// std::set<sf::Vector2f> множество координат фигур, может быть полезно, когда их мало, но по ним надо итерировать.
	uint16_t flags; //рокировался ли король, какая пешка сейчас сделала и, может, что-то ещё.
	uint16_t timer = 50; // обратный отсчёт до ничьи при отсутствии значимых ходов
	std::list<step> story; //история ходов
	enum gameState {
		WHITE_TURN,
		BLACK_TURN,
		WHITE_WON,
		BLACK_WON,
		DRAW
	} state; // состояние игры
public:
	enum type {
		EMPTY,
		DEFAULT
	};
	board(type t);
	figure& at(uint8_t x, uint8_t y);
	static uint8_t getField(uint8_t rank, uint8_t file);
	static std::pair<uint8_t, uint8_t> setField(uint8_t field) ;
	void playStep(step s); // применение хода

	void undoStep(step s) {

	}; // отмена хода

//  int checkKingSafety(){
//  
//  }

	int tryStep(sf::Vector2i from, sf::Vector2i to);
	// обработчик произвольных ходов игрока. возвращает код-результата хода:
	// произведён ли ход вообще (!), повышена ли фигура (!), убита ли фигура, произведён ли шах / мат.
	// могут быть проблемы с обработкой хода типа повышения пешки, нужно будет передавать доп аргумент - выбранная фигура.
	// поэтому предлагаю сделать метод, через который можно передавать информацию о выбранной фигуре и, возможно, предложении ничьи / признании поражения
	// tryStep при повышении пешки не будет переключаться на WHITE/BLACK_TURN, а будет ждать call с кодом фигуры (заранее оповестив кодом об этом)
	int call(int code);
	//и, соответственно, call тоже возвращает код на всякий случай.
	std::list<step> getPossibleSteps() const;
	float eval() const; // оцнка состояния
	step getBestStep(); // возврат лучшего хода
	// вроде бы, основное описал, надо, конечно, добавить figure getAt(sf::Vector2i pos / int x, int y) const;, bool isInside(sf::Vector2) const;,
	// static bool isValid(step); и т.п. но это уже вамм решать
	// можно добавить ещё геттеры всякие, но это не такая большая проблема
};

//P.S. господи, эти всратые вещи типа рокировки, взятия на проходе и выбора фигуры при повышении добавляют столько мороки
//в кастомных шахматах надо всё это отключить