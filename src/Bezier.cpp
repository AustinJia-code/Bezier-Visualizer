#include "Bezier.hpp"
#include <assert.h>

Bezier::Bezier (const std::vector<Vec2D>& vec)
{
  control_points = vec;
  child = nullptr;
  t = 0;
  Vec2D point (0, 0);
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
    getPointAtT (t, true);
  }
}

Vec2D Bezier::getPointAtT (float t, bool override)
{
  if (!override && this->t == t)
    return point;

  // TODO: Free all children

  this->t = t;
  std::vector<Vec2D> child_vec;

  // add recursive push
  for (int i = 0; i < control_points.size() - 1; i++)
    child_vec.push_back (Vec2D::lerp (control_points[i],
                                      control_points[i + 1],
                                      t));

  // Create child and return point
  child = new Bezier (child_vec, t);
  point = child->getPointAtT (t);
  return point;
}

Vec2D Bezier::getPointAtT (float t)
{
  return getPointAtT (t, false);
}

const std::vector<Vec2D>& Bezier::getControlPoints() const
{
  return control_points;
}

const Bezier* Bezier::getChild() const
{
  return child;
}