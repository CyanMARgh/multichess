#include "box2.h"

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return {lhs.x * rhs.x, lhs.y * rhs.y};
}
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return {lhs.x / rhs.x, lhs.y / rhs.y};
}

box2::box2(sf::Vector2f bottomLeft, sf::Vector2f topRight) :bottomLeft(bottomLeft), topRight(topRight) { }
box2::box2(float l, float b, float r, float t) :bottomLeft(sf::Vector2f(l, b)), topRight(sf::Vector2f(r, t)) { }
box2::box2(sf::FloatRect rect) {
	bottomLeft = {rect.left, rect.top - rect.height};
	topRight = {rect.left + rect.width, rect.top};
}
box2 box2::unit() {
	return {0, 0, 1, 1};
}
box2 box2::inv() const {
	return {bottomLeft / (bottomLeft - topRight), (sf::Vector2f(1, 1) - bottomLeft) / (topRight - bottomLeft)};
}
void box2::print() const {
	std::cout << ("((" + std::to_string(bottomLeft.x) + ", " + std::to_string(bottomLeft.y) +
			"), " + std::to_string(topRight.x) + ", " + std::to_string(topRight.y) + "))");
}

sf::Vector2f operator*(const box2& box, const sf::Vector2f& p) {
	return p * (box.topRight - box.bottomLeft) + box.bottomLeft;
}
box2 operator*(const box2& boxl, const box2& boxr) {
	return {boxl * boxr.bottomLeft, boxl * boxr.topRight};
}
box2 operator/(const box2& boxl, const box2& boxr) {
	return boxl * boxr.inv();
}

float box2::left() const {
	return bottomLeft.x;
}
float box2::top() const {
	return topRight.y;
}
float box2::right() const {
	return topRight.x;
}
float box2::bottom() const {
	return bottomLeft.y;
}

sf::Vector2f box2::topLeft() const {
	return {bottomLeft.x, topRight.y};
}
sf::Vector2f box2::bottomRight() const {
	return {topRight.x, bottomLeft.y};
}
box2::operator sf::IntRect() const {
	return {(int)bottomLeft.x, (int)bottomLeft.y, (int)topRight.x, (int)topRight.y};
}

bool box2::isInside(sf::Vector2f p) const {
	return p.x > bottomLeft.x && p.y > bottomLeft.y && p.x < topRight.x && p.y < topRight.y;
}

float box2::height() const {
	return topRight.y - bottomLeft.y;
}
float box2::width() const {
	return topRight.x - bottomLeft.x;
}

sf::Vector2f box2::center() const {
	return (topRight + bottomLeft) * .5f;
}
sf::Vector2f box2::rad() const {
	return (topRight - bottomLeft) * .5f;
}
