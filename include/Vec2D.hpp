#ifndef VEC2D_HPP
#define VEC2D_HPP

#include <iostream>

class Vec2D
{
  public:
    float x;                                    // x position
    float y;                                    // y position
    
    /**
     * Default constructor
     */
    Vec2D ();

    /**
     * Constructor
     * @param x,y values for x and y
     */
    Vec2D (float x, float y);

    /**
     * Copy constructor
     * @param other Vec2D to copy
     */
    Vec2D (const Vec2D& other);

    /**
     * Returns new Vec2D representing scalar multiplication
     * @param scalar scalar to scale x and y by
     */
    Vec2D operator* (float scalar) const;

    /**
     * Returns new Vec2D representing scalar multiplication
     * @param scalar scalar to scale x and y by
     * @param vec Vec2D to scale 
     */
    friend Vec2D operator* (float scalar, Vec2D vec);

    /**
     * Returns new Vec2D representing vector addition
     * @param other Vec2D to add to this
     */
    Vec2D operator+ (const Vec2D& other) const;

    /**
     * Returns new Vec2D representing vector subtraction
     * @param other Vec2D to subtract from this
     */
    Vec2D operator- (const Vec2D& other) const;

    /**
     * Print statement
     * @param os output stream to put print on
     * @param obj Vec2D to print
     */
    friend std::ostream& operator<< (std::ostream& os, const Vec2D& obj);

    /**
     * Returns a new Vec2D representing a linear interpolation on two vectors
     * @param a first vector
     * @param b second vector
     * @param t the time step between the two vectors to evaluate
     */
    static Vec2D lerp (const Vec2D& a, const Vec2D& b, float t);
};

#endif // VEC2D_HPP
