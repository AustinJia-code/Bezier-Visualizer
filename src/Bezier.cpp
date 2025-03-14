#include "Bezier.hpp"
#include <assert.h>

Bezier::~Bezier() {
  delete child;
}

// Recursive constructor
Bezier::Bezier (const std::vector<Vec2D>& vec, float t) : t (t)
{
  assert (vec.size() > 1);
  control_points = vec;
  if (vec.size () == 2) {
    child = nullptr;
    point = Vec2D::lerp (vec.at (0), vec.at (1), t);
  } else {
    get_point_at_T (t, true);
  }
}

void Bezier::set_control_points (const std::vector<Vec2D>& vec) {
  Bezier (vec, this->t);
}

Vec2D Bezier::get_point_at_T (float t, bool override)
{
  if (!override && this->t == t || control_points.size() < 2)
    return point;
  
  this->t = t;
  if (control_points.size() == 2) {
    this->t = t;
    point = Vec2D::lerp (control_points.at (0), control_points.at (1), t);
    return point;
  }

  std::vector<Vec2D> child_vec;

  // add recursive push
  for (int i = 0; i < control_points.size() - 1; i++)
    child_vec.push_back (Vec2D::lerp (control_points[i],
                                      control_points[i + 1],
                                      t));

  // Create child and return point
  child = new Bezier (child_vec, t);
  point = child->get_point_at_T (t);
  return point;
}

Vec2D Bezier::get_point_at_T (float t)
{
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