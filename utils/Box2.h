#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <iostream>

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs);
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs);

struct Box2 {
	sf::Vector2f topRight, bottomLeft;

	Box2() = default;
	Box2(sf::FloatRect rect);
	Box2(sf::Vector2f bottomLeft, sf::Vector2f topRight);
	Box2(float l, float b, float r, float t);

	static Box2 Unit();
	Box2 Inv() const;
	float Left() const;
	float Top() const;
	float Right() const;
	float Bottom() const;

	sf::Vector2f Rad() const;
	sf::Vector2f Rad2() const;
	sf::Vector2f Center() const;

	float Height() const;
	float Width() const;

	sf::Vector2f TopLeft() const;
	sf::Vector2f BottomRight() const;
	void Print() const;
	explicit operator sf::IntRect() const;

	bool IsInside(sf::Vector2f p) const;
};

sf::Vector2f operator*(const Box2& box, const sf::Vector2f& p);
Box2 operator*(const Box2& boxl, const Box2& boxr);
Box2 operator/(const Box2& boxl, const Box2& boxr);
