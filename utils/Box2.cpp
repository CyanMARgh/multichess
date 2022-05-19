#include "Box2.h"

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return {lhs.x * rhs.x, lhs.y * rhs.y};
}
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return {lhs.x / rhs.x, lhs.y / rhs.y};
}

Box2::Box2(sf::Vector2f bottomLeft, sf::Vector2f topRight) :bottomLeft(bottomLeft), topRight(topRight) { }
Box2::Box2(float l, float b, float r, float t) :bottomLeft(sf::Vector2f(l, b)), topRight(sf::Vector2f(r, t)) { }
Box2::Box2(sf::FloatRect rect) {
	bottomLeft = {rect.left, rect.top - rect.height};
	topRight = {rect.left + rect.width, rect.top};
}
Box2 Box2::Unit() {
	return {0, 0, 1, 1};
}
Box2 Box2::Inv() const {
	return {bottomLeft / (bottomLeft - topRight), (sf::Vector2f(1, 1) - bottomLeft) / (topRight - bottomLeft)};
}
void Box2::Print() const {
	std::cout << ("((" + std::to_string(bottomLeft.x) + ", " + std::to_string(bottomLeft.y) +
			"), " + std::to_string(topRight.x) + ", " + std::to_string(topRight.y) + "))");
}

sf::Vector2f operator*(const Box2& box, const sf::Vector2f& p) {
	return p * (box.topRight - box.bottomLeft) + box.bottomLeft;
}
Box2 operator*(const Box2& boxl, const Box2& boxr) {
	return {boxl * boxr.bottomLeft, boxl * boxr.topRight};
}
Box2 operator/(const Box2& boxl, const Box2& boxr) {
	return boxl * boxr.Inv();
}

float Box2::Left() const {
	return bottomLeft.x;
}
float Box2::Top() const {
	return topRight.y;
}
float Box2::Right() const {
	return topRight.x;
}
float Box2::Bottom() const {
	return bottomLeft.y;
}

sf::Vector2f Box2::TopLeft() const {
	return {bottomLeft.x, topRight.y};
}
sf::Vector2f Box2::BottomRight() const {
	return {topRight.x, bottomLeft.y};
}
Box2::operator sf::IntRect() const {
	return {(int)bottomLeft.x, (int)bottomLeft.y, (int)topRight.x, (int)topRight.y};
}

bool Box2::IsInside(sf::Vector2f p) const {
	return p.x > bottomLeft.x && p.y > bottomLeft.y && p.x < topRight.x && p.y < topRight.y;
}

float Box2::Height() const {
	return topRight.y - bottomLeft.y;
}
float Box2::Width() const {
	return topRight.x - bottomLeft.x;
}

sf::Vector2f Box2::Center() const {
	return (topRight + bottomLeft) * .5f;
}
sf::Vector2f Box2::Rad() const {
	return (topRight - bottomLeft) * .5f;
}
sf::Vector2f Box2::Rad2() const {
	return topRight - bottomLeft;
}
