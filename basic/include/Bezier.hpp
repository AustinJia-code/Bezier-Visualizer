#include "Vec2D.hpp"
#include <vector>

class Bezier
{
  private:
    std::vector<Vec2D> control_points;  // points definining this curve
    Bezier* child;                      // Bezier from interpolating on this curve
    float t;                            // The current time step
    Vec2D point;                        // The point along this curve at time t
    
    /**
     * Returns a the point on this Bezier at time t
     * @param t time step at which to calculate point
     * @param override conditional that forces recalculation of point
     */
    Vec2D get_point_at_T (float t, bool override);

  public:
    /**
     * Destructor
     */
    ~Bezier ();

    /**
     * Constructor
     * @param vec list of graph control points
     * @param t initial time step
     */
    Bezier (const std::vector<Vec2D>& vec, float t);

    /**
     * Replaces control_points
     * @param vec list to set control_points to
     */
    void set_control_points (const std::vector<Vec2D>& vec);

    /**
     * Returns a point along the bezier curve
     * @param t time step to evaluate this Bezier on
     */
    Vec2D get_point_at_T (float t);

    /**
     * Returns control points, const version
     */
    const std::vector<Vec2D>& get_control_points() const;

    /**
     * Returns control points, modifiable version
     */
    std::vector<Vec2D>& get_control_points();

    /**
     * Returns the Bezier defined by interpolating on this Bezier
     */
    const Bezier* getChild() const;
};