#include <algorithm>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "Vec2D.hpp"

float SCALE = 2.0;                              // Used to increase resolution

/*************** Canvas *****************/ 
unsigned int CANVAS_MAX_DIM_PX = 800 * SCALE;   // Max pixel width of the canvas
sf::Color lightGrey (100, 100, 100);

/*************** Bezier *****************/ 
unsigned int R_PX = 2.5 * SCALE;                // Standard circle radius
unsigned int W_PX = 2 * SCALE;                  // Standard line width
float STEP = 0.001;                             // Step granularity
sf::Color c1 (65, 99, 99);                      // #659999
sf::Color c2 (244, 79, 31);                     // #f4791f

/*************** Graph *****************/ 
int X_MIN = -3;                                 // Minimum x coordinate
int X_MAX = 3;                                  // Maximum x coordinate
int Y_MIN = -3;                                 // Minimum y coordinate
int Y_MAX = 3;                                  // Maximum y coordinate

unsigned int UNIT_PX                            // Graph unit pixel length
              = CANVAS_MAX_DIM_PX / (std::max (X_MAX - X_MIN, Y_MAX - Y_MIN));
unsigned int X_PX = UNIT_PX * (X_MAX - X_MIN);  // Total x width pixels
unsigned int Y_PX = UNIT_PX * (Y_MAX - Y_MIN);  // Total y height pixels

/**
 * Convert x graph coordinate to x pixels
 * @param x graph coordinate to convert to pixels
 */
int X_TO_CANVAS (float x) {
  return (x - X_MIN) * UNIT_PX;
}

/**
 * Convert x pixels to x graph coordinate
 * @param cx x pixels to convert to graph coordinate
 */
float CANVAS_TO_X (int cx) {
  return static_cast<float> (cx) / (UNIT_PX / SCALE) + X_MIN;
}

/**
 * Convert y graph coordinate to y pixels
 * @param y graph coordinate to convert to pixels
 */
int Y_TO_CANVAS (float y) {
  return Y_PX - ((y - Y_MIN) * UNIT_PX);
}

/**
 * Convert y pixels to y graph coordinate
 * @param cy y pixels to convert to graph coordinate 
 */
float CANVAS_TO_Y (int cy) {
  return (Y_PX / SCALE - static_cast<float> (cy)) / (UNIT_PX / SCALE) + Y_MIN;
}

/**
 * Convert graph coordinates to canvas pixels
 * @param vec point to convert
 */
sf::Vector2f VEC_TO_CANVAS (const Vec2D vec) {
  return {static_cast<float> (X_TO_CANVAS (vec.x)), 
          static_cast<float> (Y_TO_CANVAS (vec.y))};
}

/**
 * Convert canvas pixels to graph coordinates
 * @param cx x pixel to convert
 * @param cy y pixel to convert
 */
Vec2D CANVAS_TO_VEC (int cx, int cy) {
  return Vec2D (CANVAS_TO_X (cx), CANVAS_TO_Y (cy));
}