#include "Vec2D.hpp"
#include <vector>

class Bezier
{
  private:
    std::vector<Vec2D> control_points;
    Bezier* child;
    float t;
    Vec2D point;
    Vec2D getPointAtT (float t, bool override);

  public:
    Bezier (const std::vector<Vec2D>& vec);
    Bezier (const std::vector<Vec2D>& vec, float t);
    Vec2D getPointAtT (float t);
    const std::vector<Vec2D>& getControlPoints() const;
    const Bezier* getChild() const;
};