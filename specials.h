#pragma once
#include <SFML/Graphics/Rect.hpp>

namespace cm {
	bool valid(sf::Vector2i v, sf::Vector2i s);
	bool valid(sf::Vector2i v, sf::Vector2u s);
}
