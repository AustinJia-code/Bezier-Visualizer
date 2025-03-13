#ifndef VEC2D_HPP
#define VEC2D_HPP

#include <iostream>

class Vec2D
{
  public:
    // Fields
    float x;
    float y;
    
    // Constructors
    Vec2D ();
    Vec2D (float x, float y);
    Vec2D (const Vec2D& other);

    // Operators
    Vec2D operator* (float scalar) const;
    friend Vec2D operator* (float scalar, Vec2D vec);
    Vec2D operator+ (const Vec2D& other) const;
    Vec2D operator- (const Vec2D& other) const;
    friend std::ostream& operator<< (std::ostream& os, const Vec2D& obj);

    // Static functions
    static Vec2D lerp(const Vec2D& a, const Vec2D& b, float t);
};

#endif // VEC2D_HPP
