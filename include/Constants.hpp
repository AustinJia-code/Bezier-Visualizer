#include <algorithm>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "Vec2D.hpp"

float SCALE = 2.0;

/*************** Canvas *****************/ 
unsigned int CANVAS_MAX_DIM_PX = 800 * SCALE;
sf::Color lightGrey (100, 100, 100);

/*************** Graph *****************/ 
int X_MIN = -3;
int X_MAX = 3;
int Y_MIN = -3;
int Y_MAX = 3;
// Width of one graph unit in pixels
unsigned int UNIT_WIDTH_PX = CANVAS_MAX_DIM_PX / (std::max (X_MAX - X_MIN, Y_MAX - Y_MIN));
// Total width of x and y dimensions in pixels
unsigned int X_WIDTH_PX = UNIT_WIDTH_PX * (X_MAX - X_MIN);
unsigned int Y_WIDTH_PX = UNIT_WIDTH_PX * (Y_MAX - Y_MIN);
// Convert from graph coordinates to canvas coordinates
int X_TO_CANVAS (float x) {
  float rel_pos = x - X_MIN;
  return rel_pos * UNIT_WIDTH_PX;
}
float CANVAS_TO_X (int cx) {
  return static_cast<float> (cx) / (UNIT_WIDTH_PX / SCALE) + X_MIN;
}
int Y_TO_CANVAS (float y) {
  float rel_pos = y - Y_MIN;
  return Y_WIDTH_PX - (rel_pos * UNIT_WIDTH_PX);
}
float CANVAS_TO_Y (int cy) {
  return (Y_WIDTH_PX / SCALE - static_cast<float> (cy)) / (UNIT_WIDTH_PX / SCALE) + Y_MIN;
}
sf::Vector2f VEC_TO_CANVAS (const Vec2D vec) {
  return {static_cast<float> (X_TO_CANVAS (vec.x)), 
          static_cast<float> (Y_TO_CANVAS (vec.y))};
}
Vec2D CANVAS_TO_VEC (int cx, int cy) {
  return Vec2D (CANVAS_TO_X (cx), CANVAS_TO_Y (cy));
}

/*************** Bezier *****************/ 
// Circle Radius
unsigned int R_PX = 2.5 * SCALE;
// Line Weight
unsigned int W_PX = 2 * SCALE;
float STEP = 0.001;