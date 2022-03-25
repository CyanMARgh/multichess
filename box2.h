#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <iostream>

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs);
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs);

struct box2 {
	sf::Vector2f topRight, bottomLeft;

	box2() = default;
	box2(sf::FloatRect rect);
	box2(sf::Vector2f bottomLeft, sf::Vector2f topRight);
	box2(float l, float b, float r, float t);

	static box2 unit();
	box2 inv() const;
	float left() const;
	float top() const;
	float right() const;
	float bottom() const;

	sf::Vector2f rad() const;
	sf::Vector2f center() const;

	float height() const;
	float width() const;

	sf::Vector2f topLeft() const;
	sf::Vector2f bottomRight() const;
	void print() const;
	explicit operator sf::IntRect() const;

	bool isInside(sf::Vector2f p) const;
};

sf::Vector2f operator*(const box2& box, const sf::Vector2f& p);
box2 operator*(const box2& boxl, const box2& boxr);
box2 operator/(const box2& boxl, const box2& boxr);
