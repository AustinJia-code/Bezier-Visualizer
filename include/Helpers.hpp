#include "Constants.hpp"
#include "Bezier.hpp"

void build_line (sf::ConvexShape& line, sf::Vector2f start, sf::Vector2f end, float thickness, sf::Color color) {
  // Compute direction vector
  sf::Vector2f direction = end - start;

  // Normalize it
  float length = std::sqrt (direction.x * direction.x + direction.y * direction.y);
  if (length == 0) return;
  direction /= length;

  // Compute perpendicular normal
  sf::Vector2f normal (-direction.y, direction.x);

  // Scale normal to half the thickness
  normal *= (thickness * SCALE / 2);

  // Set up a ConvexShape with 4 points (quad)
  line.setPointCount(4);
  line.setPoint(0, start - normal);
  line.setPoint(1, start + normal);
  line.setPoint(2, end + normal);
  line.setPoint(3, end - normal);
  line.setFillColor(color);
};

void draw_point (sf:: RenderTexture& foreground, sf::Vector2f pos, int size, sf::Color color)
{
  sf::CircleShape point (size);
  point.setOrigin(size, size);
  point.setPosition (pos);
  point.setFillColor (color);
  foreground.draw (point);
}

/**
 * Draws a unit grid onto background
 */
void draw_grid (sf::RenderTexture& background, const std::vector<Vec2D>& control_points)
{
  // Y lines
  for (int i = 0; i <= X_WIDTH_PX / UNIT_WIDTH_PX; i++) {
    int x = UNIT_WIDTH_PX * i;
    bool mid = i == (X_WIDTH_PX / UNIT_WIDTH_PX / 2);
    sf::ConvexShape line;
    sf::Vector2f start = {static_cast<float> (x), static_cast<float> (Y_TO_CANVAS (Y_MIN))};
    sf::Vector2f end = {static_cast<float> (x), static_cast<float> (Y_TO_CANVAS (Y_MAX))};
    build_line (line, start, end, mid ? W_PX : W_PX * 0.5, lightGrey);
    background.draw (line);
  }

  // X lines
  for (int i = 0; i <= Y_WIDTH_PX / UNIT_WIDTH_PX; i++) {
    int y = UNIT_WIDTH_PX * i;
    bool mid = i == (X_WIDTH_PX / UNIT_WIDTH_PX / 2);
    sf::ConvexShape line;
    sf::Vector2f start = {static_cast<float> (X_TO_CANVAS (X_MIN)), static_cast<float> (y)};
    sf::Vector2f end = {static_cast<float> (X_TO_CANVAS (X_MAX)), static_cast<float> (y)};
    build_line (line, start, end, mid ? W_PX : W_PX * 0.5, lightGrey);
    background.draw (line);
  }

  // Control points
  for (int i = 0; i < control_points.size(); i++)
    draw_point (background, VEC_TO_CANVAS (control_points.at (i)), R_PX * 2, sf::Color::Green);
}

void draw_control_lines (sf::RenderTexture& foreground, const Bezier& bezier, int color, int color_step)
{
  const std::vector<Vec2D> control_points = bezier.get_control_points();
  for (size_t i = 0; i < control_points.size() - 1; i++) {
    sf::ConvexShape line;
    sf::Vector2f start = VEC_TO_CANVAS (control_points[i]);
    sf::Vector2f end = VEC_TO_CANVAS (control_points[i + 1]);
    sf::Color clr = color == 0 ? sf::Color::Green : sf::Color (color, 256 - color, 0);
    build_line (line, start, end, W_PX, clr);
    foreground.draw(line);
  }

  if (bezier.getChild() != nullptr && bezier.get_control_points().size() > 2)
    draw_control_lines (foreground, *bezier.getChild(), color + color_step, color_step);
}