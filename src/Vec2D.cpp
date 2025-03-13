#include "Vec2D.hpp"

// Constructors
Vec2D::Vec2D () : x (0), y (0) {}
Vec2D::Vec2D (float x, float y) : x (x), y (y) {}
Vec2D::Vec2D (const Vec2D& other) : x (other.x), y (other.y) {}

// Operators
Vec2D Vec2D::operator* (float scalar) const
{
  return Vec2D (x * scalar, y * scalar);
}

Vec2D operator* (float scalar, Vec2D vec)
{
  return vec * scalar; 
}

Vec2D Vec2D::operator+ (const Vec2D& other) const
{
  return Vec2D (x + other.x, y + other.y);
}

Vec2D Vec2D::operator- (const Vec2D& other) const 
{
  return Vec2D (x - other.x, y - other.y);
}

std::ostream& operator<< (std::ostream& os, const Vec2D& obj)
{
  os << "(" << obj.x << ", " << obj.y << ")";
  return os;
}

// Utility Functions
Vec2D Vec2D::lerp (const Vec2D& a, const Vec2D& b, float t)
{
  return (a * (1.0 - t)) + (b * (t));
}