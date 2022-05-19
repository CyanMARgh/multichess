#pragma once
#include <SFML/Graphics/Rect.hpp>

namespace cm {
	bool Valid(sf::Vector2i v, sf::Vector2i s);
	bool Valid(sf::Vector2i v, sf::Vector2u s);
}
