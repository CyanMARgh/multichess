#include "utils.h"

bool cm::Valid(sf::Vector2i v, sf::Vector2i s) {
	return v.x >= 0 && v.y >= 0 && v.x < s.x && v.y < s.y;
}
bool cm::Valid(sf::Vector2i v, sf::Vector2u s) {
	return v.x >= 0 && v.y >= 0 && v.x < (int)s.x && v.y < (int)s.y;
}