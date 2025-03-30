#include "Constants.hpp"
#include "Bezier.hpp"

/**
 * Builds a line and stores it
 * @param line Object to store data in
 * @param start Canvas start pixel of the line
 * @param end Canvas end pixel of the line
 * @param thickness Pixel thickness of the line to create
 * @param color Color of the line to create
 */
void build_line (sf::ConvexShape& line, sf::Vector2f start, sf::Vector2f end, 
                float thickness, sf::Color color)
{
  // Compute direction vector
  sf::Vector2f direction = end - start;

  // Normalize it
  float length = std::sqrt (direction.x * direction.x + 
                            direction.y * direction.y);
  if (length == 0) 
    return;

  direction /= length;

  // Compute perpendicular normal
  sf::Vector2f normal (-direction.y, direction.x);

  // Scale normal to half the thickness
  normal *= (thickness * SCALE / 2);

  // Set up a ConvexShape with 4 points
  line.setPointCount (4);
  line.setPoint (0, start - normal);
  line.setPoint (1, start + normal);
  line.setPoint (2, end + normal);
  line.setPoint (3, end - normal);
  line.setFillColor (color);
};

/**
 * Draws circle at a point on a RenderTexture, but does not display it
 * @param foreground RenderTexture on which to draw the point
 * @param pos Canvas pixel coordinates at which to draw the point
 * @param size Radius of the circle
 * @param color Color of the circle to draw
 */
void draw_point (sf:: RenderTexture& foreground, sf::Vector2f pos, int size, 
                 sf::Color color)
{
  sf::CircleShape point (size);
  point.setOrigin (size, size);
  point.setPosition (pos);
  point.setFillColor (color);
  foreground.draw (point);
}

/**
 * Draws a unit grid onto background, along with the initial control points
 * @param background RenderTexture on which to draw the grid and control points
 * @param control_points vector of graph coordinates for control points
 */
void draw_grid (sf::RenderTexture& background, const std::vector<Vec2D>& control_points)
{
  // Y lines
  for (int i = 0; i <= X_PX / UNIT_PX; i++)
    {
      int x = UNIT_PX * i;
      bool mid = i == (X_PX / UNIT_PX / 2);

      sf::ConvexShape line;
      sf::Vector2f start = {static_cast<float> (x), 
                            static_cast<float> (Y_TO_CANVAS (Y_MIN))};
      sf::Vector2f end = {static_cast<float> (x), 
                          static_cast<float> (Y_TO_CANVAS (Y_MAX))};

      // make origin line thicker
      build_line (line, start, end, mid ? W_PX : W_PX * 0.5, light_grey);
      background.draw (line);
    }

  // X lines
  for (int i = 0; i <= Y_PX / UNIT_PX; i++) 
    {
      int y = UNIT_PX * i;
      bool mid = i == (X_PX / UNIT_PX / 2);

      sf::ConvexShape line;
      sf::Vector2f start = {static_cast<float> (X_TO_CANVAS (X_MIN)), 
                            static_cast<float> (y)};
      sf::Vector2f end = {static_cast<float> (X_TO_CANVAS (X_MAX)), 
                          static_cast<float> (y)};
    
      // make origin line thicker
      build_line (line, start, end, mid ? W_PX : W_PX * 0.5, light_grey);
      background.draw (line);
    }

  // Control points
  for (int i = 0; i < control_points.size(); i++)
    draw_point (background, VEC_TO_CANVAS (control_points.at (i)), 
                R_PX * 2, C1);
}

/**
 * Helper for draw_control_lines
 * Interpolates between two colors to form a gradient
 * @param t the time step to evaluate
 * @param c1 the first color
 * @param c2 the second color
 */
sf::Color interpolate_color (float t, const sf::Color& c1, const sf::Color& c2)
{
  return sf::Color (
              static_cast<sf::Uint8> ((1 - t) * c1.r + t * (c2.r)),
              static_cast<sf::Uint8> ((1 - t) * c1.g + t * (c2.g)),
              static_cast<sf::Uint8> ((1 - t) * c1.b + t * (c2.b))
             );
}

/**
 * Recursively draws all control lines of a Bezier curve and it's children
 * @param foreground RenderTexture one which to draw lines
 * @param bezier Parent bezier who's control lines are drawn
 * @param color Initial color of control lines
 * @param color_step Value of color change per step
 */
void draw_control_lines (sf::RenderTexture& foreground, const Bezier& bezier, 
                         float t, float step)
{
  // Interpolate between colors
  sf::Color clr = interpolate_color (t, C1, C2);

  const std::vector<Vec2D> control_points = bezier.get_control_points ();

  // Draw all control lines
  for (size_t i = 0; i < control_points.size() - 1; i++)
    {
      sf::ConvexShape line;
      sf::Vector2f start = VEC_TO_CANVAS (control_points[i]);
      sf::Vector2f end = VEC_TO_CANVAS (control_points[i + 1]);
      build_line (line, start, end, W_PX, clr);
      foreground.draw (line);
    }

  // Recurse, modify color
  if (bezier.getChild() != nullptr && bezier.get_control_points().size() > 2)
    draw_control_lines (foreground, *bezier.getChild(), t + step, step);
}