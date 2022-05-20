#pragma once
#include "SFML/System/Vector2.hpp"
#include "cstdint"
#include <vector>
#include <list>
#include <string>

// namespace вместо enum-а, чтобы можно было проще проводить побитовые операции
typedef uint8_t Figure;
namespace figures {
    const Figure
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

struct Step {
    bool side; // Кто ходит (черные/белые)
    bool type; // Обычный ход - 0, особый (рокировка / взятие на проходе / превращение) - 1
    struct Part {
//      sf::Vector2i from, to;
        uint8_t from, to;
        Figure f;
    }; // Part ~ передвинуть фигуру из точки from в точку to (и поставить поверх фигуры f)

//  struct Part {   // альтернативный вариант ~ заменить фигуру на At с bef на aft. не уверен, какой вариант лучше.
//		sf::Vector2i At;
//		Figure bef, aft;
//	};
    Part parts[2]; // в первом варианте хватит двух, во втором нужно до 4х, но второй вариант может быть более гибким.
    uint16_t timeBef; // значение таймера перед ходом.
    uint16_t flagsBef; // значение флагов перед ходом.

    std::string AsNotation() const; // привести к виду шахматной нотации
};

class Board {
    sf::Vector2i size = {8, 8};
    std::vector<Figure> map; //массив фигур
    // std::set<sf::Vector2f> множество координат фигур, может быть полезно, когда их мало, но по ним надо итерировать.
    uint16_t flags; //рокировался ли король, какая пешка сейчас сделала и, может, что-то ещё.
    uint16_t timer = 50; // обратный отсчёт до ничьи при отсутствии значимых ходов
    std::list<Step> story; //история ходов
    enum State {
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
    Board(type t);
    Figure& At(uint8_t x, uint8_t y);
    Figure At(uint8_t x, uint8_t y) const;
    static uint8_t GetField(uint8_t rank, uint8_t file);
    static std::pair<uint8_t, uint8_t> setField(uint8_t field) ;
    void PlayStep(Step s); // применение хода

    void UndoStep(Step s) {

    }; // отмена хода

//  int checkKingSafety(){
//
//  }

    int TryStep(sf::Vector2i from, sf::Vector2i to);
    // обработчик произвольных ходов игрока. возвращает код-результата хода:
    // произведён ли ход вообще (!), повышена ли фигура (!), убита ли фигура, произведён ли шах / мат.
    // могут быть проблемы с обработкой хода типа повышения пешки, нужно будет передавать доп аргумент - выбранная фигура.
    // поэтому предлагаю сделать метод, через который можно передавать информацию о выбранной фигуре и, возможно, предложении ничьи / признании поражения
    // TryStep при повышении пешки не будет переключаться на WHITE/BLACK_TURN, а будет ждать call с кодом фигуры (заранее оповестив кодом об этом)
    int call(int code);
    //и, соответственно, call тоже возвращает код на всякий случай.
    std::list<Step> GetPossibleSteps() const;
    float Eval() const; // оцнка состояния
    Step GetBestStep(); // возврат лучшего хода
    // вроде бы, основное описал, надо, конечно, добавить Figure getAt(sf::Vector2i pos / int x, int y) const;, bool IsInside(sf::Vector2) const;,
    // static bool isValid(Step); и т.п. но это уже вамм решать
    // можно добавить ещё геттеры всякие, но это не такая большая проблема
};

//P.S. господи, эти всратые вещи типа рокировки, взятия на проходе и выбора фигуры при повышении добавляют столько мороки
//в кастомных шахматах надо всё это отключить