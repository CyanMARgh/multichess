#include "specials.h"

bool cm::valid(sf::Vector2i v, sf::Vector2i s) {
	return v.x >= 0 && v.y >= 0 && v.x < s.x && v.y < s.y;
}
bool cm::valid(sf::Vector2i v, sf::Vector2u s) {
	return v.x >= 0 && v.y >= 0 && v.x < s.x && v.y < s.y;
}