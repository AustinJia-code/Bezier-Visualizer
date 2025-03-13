#include <algorithm>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "Vec2D.hpp"

/*************** Canvas *****************/ 
unsigned int CANVAS_MAX_DIM_PX = 800;
sf::Color lightGrey(80, 80, 80);

/*************** Graph *****************/ 
int X_MIN = -1;
int X_MAX = 5;
int Y_MIN = -4;
int Y_MAX = 2;
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
int Y_TO_CANVAS (float y) {
  float rel_pos = y - Y_MIN;
  return Y_WIDTH_PX - (rel_pos * UNIT_WIDTH_PX);
}
sf::Vector2f VEC_TO_CANVAS (const Vec2D vec) {
  return {static_cast<float> (X_TO_CANVAS (vec.x)), 
          static_cast<float> (Y_TO_CANVAS (vec.y))};
}

/*************** Bezier *****************/ 
unsigned int R_PX = 2;
unsigned int W_PX = 3;
float STEP = 0.001;

/*************** Misc Helpers *****************/ 
void buildLine (sf::ConvexShape& line, sf::Vector2f start, sf::Vector2f end, float thickness, sf::Color color) {
  // Compute direction vector
  sf::Vector2f direction = end - start;

  // Normalize it
  float length = std::sqrt (direction.x * direction.x + direction.y * direction.y);
  if (length == 0) return;
  direction /= length;

  // Compute perpendicular normal
  sf::Vector2f normal (-direction.y, direction.x);

  // Scale normal to half the thickness
  normal *= (thickness / 2);

  // Set up a ConvexShape with 4 points (quad)
  line.setPointCount(4);
  line.setPoint(0, start - normal);
  line.setPoint(1, start + normal);
  line.setPoint(2, end + normal);
  line.setPoint(3, end - normal);
  line.setFillColor(color);
};