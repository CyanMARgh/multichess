#pragma once

#include "SFML/System/Vector2.hpp"
#include <iostream>

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs);
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs);

struct box2 {
	sf::Vector2f topRight, bottomLeft;

	box2(sf::Vector2f bottomLeft, sf::Vector2f topRight);
	box2(float b, float l, float t, float r);

	static box2 unit();
	box2 inv() const;
	void print();
};

sf::Vector2f operator*(const box2& box, const sf::Vector2f& p);
box2 operator*(const box2& boxl, const box2& boxr);
box2 operator/(const box2& boxl, const box2& boxr);