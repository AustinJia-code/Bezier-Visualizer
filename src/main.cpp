#include "Helpers.hpp"
#include "Slider.hpp"
#include <vector>

/**
 * Initialize control points for the Bezier curve
 */
std::vector<Vec2D> init_curve ()
{
  std::vector<Vec2D> control_points;

  control_points.push_back (Vec2D (-2, 1.5));
  control_points.push_back (Vec2D (-2, 0.5));
  control_points.push_back (Vec2D (-1, 0));
  control_points.push_back (Vec2D (-1.5, 2));
  control_points.push_back (Vec2D (2, 1.5));
  control_points.push_back (Vec2D (1.5, -2));

  return control_points;
}

void handle_add_point (const sf::Event& event, sf::RenderWindow& window, std::vector<Vec2D>& control_points)
{
  if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
    sf::Vector2i pos = sf::Mouse::getPosition (window);
    Vec2D temp = CANVAS_TO_VEC (pos.x, pos.y);
    if (temp.x >= X_MIN && temp.x <= X_MAX
     && temp.y >= Y_MIN && temp.y <= Y_MAX)
      control_points.push_back (temp);
  }
}

void handle_remove_point (const sf::Event& event, sf::RenderWindow& window, std::vector<Vec2D>& control_points)
{
  if (sf::Mouse::isButtonPressed (sf::Mouse::Right)) {
    sf::Vector2i pos = sf::Mouse::getPosition (window);
    int min_index = -1;
    float min_dist = INFINITY;
    for (int i = 0; i < control_points.size(); i++) {
      Vec2D click = CANVAS_TO_VEC (pos.x, pos.y);
      Vec2D point = control_points.at (i);
      float dist = std::hypot (click.x - point.x, click.y - point.y);
      if (dist < min_dist) {
        min_dist = dist;
        min_index = i;
      }
    }

    if (min_index > -1 && control_points.size() > 2 && min_dist < 0.5)
      control_points.erase (control_points.begin() + min_index);
  }
}

int main ()
{
  // Slider from t = 0 to t = 1.0, step of 0.1
  float time = 0.5;
  Slider time_slider (50, Y_WIDTH_PX - 50 * SCALE, 300, 20, 0.0, 1.0, time, SCALE);

  // Bezier init
  Bezier bezier (init_curve(), time);

  // Init window
  sf::RenderWindow window (sf::VideoMode (
                                 static_cast<unsigned int> (X_WIDTH_PX / SCALE), 
                                 static_cast<unsigned int> (Y_WIDTH_PX / SCALE)), 
                                 "Bezier Curve",
                                 sf::Style::Titlebar);
  // Set a view that maintains the original coordinate system
  sf::View zoom (sf::FloatRect (0, 0, X_WIDTH_PX, Y_WIDTH_PX));
  window.setView (zoom);

  // Create and draw background grid
  sf::RenderTexture background;
  background.create (X_WIDTH_PX, Y_WIDTH_PX);

  // Create foreground layer
  sf::RenderTexture foreground;
  foreground.create (X_WIDTH_PX, Y_WIDTH_PX);

  while (window.isOpen()) {
    // Poll events
    sf::Event event;
    while (window.pollEvent (event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::Resized) {
        // Lock the window size
        window.setSize (sf::Vector2u (static_cast<unsigned int> (X_WIDTH_PX / SCALE), 
                                      static_cast<unsigned int> (Y_WIDTH_PX / SCALE)));
      }

      // Handle slider events
      if (time_slider.handle_event (event, window))
        time = time_slider.get_value();
      else {
        handle_add_point (event, window, bezier.get_control_points ());
        handle_remove_point (event, window, bezier.get_control_points ());
      } 
    }

    // Clear window, redraw background
    window.clear(); 
    background.clear (sf::Color::White);
    draw_grid (background, bezier.get_control_points()); 
    background.display();
    sf::Sprite bgSprite (background.getTexture());
    window.draw (bgSprite);

    // Draw curve control lines
    foreground.clear (sf::Color::Transparent);
    int color_step = 256 / bezier.get_control_points().size();
    draw_control_lines (foreground, bezier, 0, color_step);

    // Draw points
    for (float t = 0; t <= time; t += STEP)
      draw_point (foreground, VEC_TO_CANVAS (bezier.get_point_at_T (t)), R_PX, sf::Color::Red);
    draw_point (foreground, VEC_TO_CANVAS (bezier.get_point_at_T (time)), R_PX * 2, sf::Color::Red);

    // Draw foreground texture
    foreground.display();
    sf::Sprite fgSprite (foreground.getTexture());
    window.draw (fgSprite);

    // Draw slider
    time_slider.draw(window);

    // Update window
    window.display();
  }
}