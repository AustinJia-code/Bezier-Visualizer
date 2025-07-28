#include "SFMLUtil.hpp"
#include "Slider.hpp"
#include <vector>
/**
 * Initialize control points for the Bezier curve
 * @return vector generated, with control points in order
 */
std::vector<Vec2D> init_curve ()
{
  std::vector<Vec2D> control_points;

  control_points.push_back (Vec2D (-2,    1.5));
  control_points.push_back (Vec2D (-2,    0.5));
  control_points.push_back (Vec2D (-1,    0));
  control_points.push_back (Vec2D (-1.5,  2));
  control_points.push_back (Vec2D (2,     1.5));
  control_points.push_back (Vec2D (1.5,   -2));

  return control_points;
}

/**
 * Adds a control point based on left-click on canvas
 * @param event input event to evaluate
 * @param window RenderWindow to evaluate mouse position on
 * @param control_points vector to add control points to 
 */
void handle_add_point (const sf::Event& event, sf::RenderWindow& window, 
                       std::vector<Vec2D>& control_points)
{
  // Event must be a left button click
  if (!sf::Mouse::isButtonPressed (sf::Mouse::Left)) 
    return;
  
  sf::Vector2i pos = sf::Mouse::getPosition (window);
  Vec2D temp = CANVAS_TO_VEC (pos.x, pos.y);
  // If the click was within canvas bounds, add it
  if (temp.x >= X_MIN && temp.x <= X_MAX && temp.y >= Y_MIN && temp.y <= Y_MAX)
    control_points.push_back (temp);
}

/**
 * Removes the nearest control point to a right-click on canvas
 * @param event input event to evaluate
 * @param window RenderWindow to evaluate mouse position on
 * @param control_points vector to remove control points from 
 */
void handle_remove_point (const sf::Event& event, sf::RenderWindow& window, 
                          std::vector<Vec2D>& control_points)
{
  // Event must be a right button click
  if (!sf::Mouse::isButtonPressed (sf::Mouse::Right))
    return;

  sf::Vector2i pos = sf::Mouse::getPosition (window);
  int min_index = -1;
  float min_dist = INFINITY;
  // Loop through control points
  for (int i = 0; i < control_points.size(); i++) 
    {
      Vec2D click = CANVAS_TO_VEC (pos.x, pos.y);
      Vec2D point = control_points.at (i);
      // Check distance
      float dist = std::hypot (click.x - point.x, click.y - point.y);
      if (dist < min_dist)
        {
          min_dist = dist;
          min_index = i;
        }
    }

  // If found a valid control point, remove it
  if (min_index > -1 && control_points.size () > 2 && min_dist < 0.5)
    control_points.erase (control_points.begin () + min_index);
}

void main_loop () {
  // Slider from t = 0 to t = 1.0, step of 0.1
  float time = 0.5;
  Slider time_slider (50, Y_PX - 50 * SCALE, 300, 20, 0.0, 1.0, time, SCALE);

  // Bezier init
  Bezier bezier (init_curve (), time);

  // Init window
  sf::RenderWindow window (sf::VideoMode (
                            static_cast<unsigned int> (X_PX / SCALE), 
                            static_cast<unsigned int> (Y_PX / SCALE)), 
                            "Bezier Curve",
                            sf::Style::Titlebar);
  // Set a view that maintains the original coordinate system
  sf::View zoom (sf::FloatRect (0, 0, X_PX, Y_PX));
  window.setView (zoom);

  // Create and draw background grid
  sf::RenderTexture background;
  background.create (X_PX, Y_PX);

  // Create foreground layer
  sf::RenderTexture foreground;
  foreground.create (X_PX, Y_PX);

  while (window.isOpen ()) 
    {
      sf::Event event;
      while (window.pollEvent (event)) 
        {
          // Check for x-out
          if (event.type == sf::Event::Closed)
            window.close ();

          // Lock the window size
          if (event.type == sf::Event::Resized)
            window.setSize (sf::Vector2u (
                              static_cast<unsigned int> (X_PX / SCALE), 
                              static_cast<unsigned int> (Y_PX / SCALE)));

          // Handle slider events
          if (time_slider.handle_event (event, window))
            {
              time = time_slider.get_value ();
            }
          // Handle mouse clicks
          else 
            {
              handle_add_point (event, window, bezier.get_control_points ());
              handle_remove_point (event, window, bezier.get_control_points ());
            } 
        }

      // Clear window, redraw background
      window.clear (); 
      background.clear (sf::Color::White);
      draw_grid (background, bezier.get_control_points ()); 
      background.display ();
      sf::Sprite bgSprite (background.getTexture ());
      window.draw (bgSprite);

      // Draw curve control lines
      foreground.clear (sf::Color::Transparent);
      int num_beziers = bezier.get_control_points ()
                              .size ();
      float t_step = 1.0 / (num_beziers - 1);
      draw_control_lines (foreground, bezier, 0, t_step);

      // Draw points
      for (float t = 0; t <= time; t += STEP)
        draw_point (foreground, VEC_TO_CANVAS (bezier.get_point_at_T (t)), 
                                              R_PX, C2);
      draw_point (foreground, VEC_TO_CANVAS (bezier.get_point_at_T (time)), 
                                            R_PX * 2, C2);

      // Draw foreground texture
      foreground.display ();
      sf::Sprite fgSprite (foreground.getTexture ());
      window.draw (fgSprite);

      // Draw slider
      time_slider.draw (window);

      // Update window
      window.display ();
    }
}

int main () 
{
  while (true)
    main_loop ();
}