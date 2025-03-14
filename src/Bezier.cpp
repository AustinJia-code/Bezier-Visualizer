#include "Bezier.hpp"
#include <assert.h>

Bezier::~Bezier() {
  delete child;
}

Bezier::Bezier (const std::vector<Vec2D>& vec, float t) : t (t)
{
  // There should always be at least two control points
  assert (vec.size() > 1);

  control_points = vec;
  // if this is a final interpolation, calculate point
  if (vec.size () == 2) {
    child = nullptr;
    point = Vec2D::lerp (vec.at (0), vec.at (1), t);
  } 
  // Get point, force calculation
  else {
    get_point_at_T (t, true);
  }
}

void Bezier::set_control_points (const std::vector<Vec2D>& vec)
{
  // Simply reinitialize with vec
  Bezier (vec, this->t);
}

Vec2D Bezier::get_point_at_T (float t, bool override)
{
  // If no override, t is same, or invalid control points, return point
  if (!override && this->t == t || control_points.size() < 2)
    return point;
  
  this->t = t;
  // If final interpolation, calculate point
  if (control_points.size() == 2) {
    this->t = t;
    point = Vec2D::lerp (control_points.at (0), control_points.at (1), t);
    return point;
  }

  std::vector<Vec2D> child_vec;

  // Interpolate points for child Bezier
  for (int i = 0; i < control_points.size() - 1; i++)
    child_vec.push_back (Vec2D::lerp (control_points[i],
                                      control_points[i + 1],
                                      t));

  // Create child and recursively return their point
  child = new Bezier (child_vec, t);
  point = child->get_point_at_T (t);
  return point;
}

Vec2D Bezier::get_point_at_T (float t)
{
  // Default to no override
  return get_point_at_T (t, false);
}

const std::vector<Vec2D>& Bezier::get_control_points() const
{
  return control_points;
}

std::vector<Vec2D>& Bezier::get_control_points()
{
  return control_points;
}

const Bezier* Bezier::getChild() const
{
  return child;
}