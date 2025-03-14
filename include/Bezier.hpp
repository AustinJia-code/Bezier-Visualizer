#include "Vec2D.hpp"
#include <vector>

class Bezier
{
  private:
    std::vector<Vec2D> control_points;
    Bezier* child;
    float t;
    Vec2D point;
    Vec2D get_point_at_T (float t, bool override);

  public:
    ~Bezier ();
    Bezier (const std::vector<Vec2D>& vec, float t);
    void set_control_points (const std::vector<Vec2D>& vec);
    Vec2D get_point_at_T (float t);
    const std::vector<Vec2D>& get_control_points() const;
    std::vector<Vec2D>& get_control_points();
    const Bezier* getChild() const;
};