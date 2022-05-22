#pragma once
#include "SFML/System/Vector2.hpp"
#include "cstdint"
#include <vector>
#include <set>
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
    int type = 0; // Обычный ход - 0, короткая рокировка - 1, длинная рокировка - 2, взятие на проходе - 3, превращение - 4.
    struct Part {
        sf::Vector2i from, to;
        Figure f;
    }; // Part ~ передвинуть фигуру из точки from в точку to (и поставить поверх фигуры f)
    Part parts[2]; // в первом варианте хватит двух, во втором нужно до 4х, но второй вариант может быть более гибким.
    //uint16_t timeBef; // значение таймера перед ходом.
    //uint16_t flagsBef; // значение флагов перед ходом.
};

class Board {
    sf::Vector2i size = {8, 8};
    std::vector<Figure> map; //массив фигур
    // std::set<sf::Vector2f> множество координат фигур, может быть полезно, когда их мало, но по ним надо итерировать.
    bool black_king = true; // способность рокироваться
    bool white_king = true;// !!! НАДО ПЕРЕНЕСТИ ВО ФЛАГИ
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
    // НУЖНО ДВА МЕТОДА, const - ДЛЯ ТОГО, ЧТОБЫ Я МОГ ОБРАЩАТЬСЯ К КЛЕТКАМ ДЛЯ ОТРИСОВКИ.
    Figure& At(uint8_t x, uint8_t y);
    Figure At(uint8_t x, uint8_t y) const;
    Figure GetState();
    static uint8_t GetField(uint8_t rank, uint8_t file);
    static std::pair<uint8_t, uint8_t> SetField(uint8_t field) ;
    std::vector<sf::Vector2i> PossibleMoves(const uint8_t& x, const uint8_t& y); // обычные ходы и превращения
    std::vector<sf::Vector2i> PossibleCastlings(const uint8_t& x, const uint8_t& y); // рокировки
    std::vector<sf::Vector2i> PossibleTOA(const uint8_t& x, const uint8_t& y); // взятия на проходе
    std::set<std::pair<int, int>> GetAttackedFields(); // какие поля сейчас атакует соперник
    // КАКОЙ std::pair<int, int>? ИСПОЛЬЗУЙ sf::Vector2u или sf::Vector2i
    void PlayStep(Step s); // применение хода
    void UndoStep(); // отмена последнего хода
    bool PlayerStep(sf::Vector2i from, sf::Vector2i to);
    void Reset();

    //МЕТОД, КОТОРЫЙ ОБРАБАТЫВАЕТ НАЖАТИЕ ИГРОКА НА КЛЕТКУ, ВОЗВРАЩАЕТ ОТВЕТ, СОДЕРЖАЩИЙ ИНФОРМАЦИЮ О РЕЗУЛЬТАТЕ НАЖАТИЯ: СОВЕРШЁН ЛИ ХОД, ОСТАЛОСЬ ИЛИ ПРОПАЛО ВЫДЕЛЕНИЕ,
    //ОЖИДАЕТСЯ ЛИ ОТ ИГРОКА ТЕПЕРЬ ВЫБОР ПОВЫШЕНИЯ ПЕШКИ
    //int Click(sf::Vector2u pos);
    //СООБЩЕНИЕ ДОСКЕ О ДЕЙСТВИИ, КОТОРОЕ НЕ ЯВЛЯЕТСЯ НАЖАТИЕМ НА КНОПКУ
    //int Call(int code);
};
