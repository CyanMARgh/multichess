#include "box2.h"

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return sf::Vector2f(lhs.x * rhs.x, lhs.y * rhs.y);
}
sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
	return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

box2::box2(sf::Vector2f bottomLeft, sf::Vector2f topRight) :bottomLeft(bottomLeft), topRight(topRight) { }
box2::box2(float b, float l, float t, float r) :bottomLeft(sf::Vector2f(b, l)), topRight(sf::Vector2f(t, r)) { }

box2 box2::unit() {
	return {0, 0, 1, 1};
}
box2 box2::inv() const {
	return {bottomLeft / (bottomLeft - topRight), (sf::Vector2f(1, 1) - bottomLeft) / (topRight - bottomLeft)};
}
void box2::print() {
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