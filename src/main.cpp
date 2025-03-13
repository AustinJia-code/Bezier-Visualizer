#include "Bezier.hpp"
#include "Constants.hpp"
#include <array>
#include <vector>

/**
 * Initialize control points for the Bezier curve
 */
std::vector<Vec2D> init_curve ()
{
  std::vector<Vec2D> control_points;
  control_points.push_back (Vec2D (0, 1));
  control_points.push_back (Vec2D (0, 0));
  control_points.push_back (Vec2D (1, 0));
  control_points.push_back (Vec2D (1, 1));
  control_points.push_back (Vec2D (4, 1));
  control_points.push_back (Vec2D (4, -3));
  return control_points;
}

/**
 * Draws a unit grid onto background
 */
void draw_grid (sf::RenderTexture& background)
{
  for (float x = 0; x <= X_WIDTH_PX; x += UNIT_WIDTH_PX) {
    sf::ConvexShape line;
    sf::Vector2f start = {x, static_cast<float> (Y_TO_CANVAS (Y_MIN))};
    sf::Vector2f end = {x, static_cast<float> (Y_TO_CANVAS (Y_MAX))};
    buildLine (line, start, end, W_PX, lightGrey);
    background.draw (line);
  }

  for (float y = 0; y <= Y_WIDTH_PX; y += UNIT_WIDTH_PX) {
    sf::ConvexShape line;
    sf::Vector2f start = {static_cast<float> (X_TO_CANVAS (X_MIN)), y};
    sf::Vector2f end = {static_cast<float> (X_TO_CANVAS (X_MAX)), y};
    buildLine (line, start, end, W_PX, lightGrey);
    background.draw (line);
  }
}

void draw_control_lines (sf::RenderTexture& foreground, const Bezier& bezier, int color, int color_step)
{
  const std::vector<Vec2D> control_points = bezier.getControlPoints();
  for (size_t i = 0; i < control_points.size() - 1; i++) {
    sf::ConvexShape line;
    sf::Vector2f start = VEC_TO_CANVAS (control_points[i]);
    sf::Vector2f end = VEC_TO_CANVAS (control_points[i + 1]);
    sf::Color clr = color == 0 ? sf::Color::Green : sf::Color (color, 200, 200);
    buildLine (line, start, end, W_PX, clr);
    foreground.draw(line);
  }

  if (bezier.getChild() != nullptr)
    draw_control_lines (foreground, *bezier.getChild(), color + color_step, color_step);
}

int main ()
{
  float master_time = 0;
  // Bezier init
  Bezier bezier (init_curve());

  sf::RenderWindow window(sf::VideoMode ({X_WIDTH_PX, Y_WIDTH_PX}), "Bezier Curve");

  // Create and draw background grid
  sf::RenderTexture background;
  background.create (X_WIDTH_PX, Y_WIDTH_PX);
  background.clear(sf::Color::White);
  draw_grid (background); 
  background.display();
  sf::Sprite bgSprite (background.getTexture());

  // Create foreground layer
  sf::RenderTexture foreground;
  foreground.create (X_WIDTH_PX, Y_WIDTH_PX);

  while (window.isOpen()) {
    // Poll events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Clear window, redraw background
    window.clear(); 
    window.draw (bgSprite);

    // Draw curve control lines
    foreground.clear(sf::Color::Transparent);
    int color_step = 256 / bezier.getControlPoints().size();
    draw_control_lines (foreground, bezier, 0, color_step);

    // Draw points
    for (float t = 0; t <= master_time; t += STEP) {
      sf::CircleShape point (R_PX);
      point.setPosition (VEC_TO_CANVAS (bezier.getPointAtT (t)));
      point.setFillColor (sf::Color::Red);
      foreground.draw (point);
    }

    // Draw foreground texture
    foreground.display();
    sf::Sprite fgSprite (foreground.getTexture());
    window.draw (fgSprite);

    // Update window
    window.display();

    master_time = master_time > 1 ? 0 : master_time += 0.01;
  }
}