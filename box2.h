#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <iostream>

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs);
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs);

struct box2 {
	sf::Vector2f topRight, bottomLeft;

	box2() = default;
	box2(sf::Vector2f bottomLeft, sf::Vector2f topRight);
	box2(float b, float l, float t, float r);

	static box2 unit();
	box2 inv() const;
	float left() const;
	float top() const;
	float right() const;
	float bottom() const;

	sf::Vector2f topLeft() const;
	sf::Vector2f bottomRight() const;
	void print();
	operator sf::IntRect() const;
};

sf::Vector2f operator*(const box2& box, const sf::Vector2f& p);
box2 operator*(const box2& boxl, const box2& boxr);
box2 operator/(const box2& boxl, const box2& boxr);