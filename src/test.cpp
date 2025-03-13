#include "Bezier.hpp"
#include <iomanip>
using namespace std;

// Point Lerp
void test1 ()
{
  Vec2D point (0, 2);
  cout << "Point: " << point << endl;
  Vec2D other (0, 4);
  cout << other + point << endl;
  cout << other * 3 << endl;
  cout << Vec2D::lerp (point, other, 0.2) << endl;
}

// D1 Bezier
void test2 ()
{
  std::vector<Vec2D> vec;
  vec.push_back (Vec2D (0, 0));
  vec.push_back (Vec2D (0, 1));
  Bezier line (vec, 0.5);
  cout << line.getPointAtT (0.5) << endl;
}

// D2 Bezier
void test3 ()
{
  /*
       * Start
       |
       |
       *------* End
   */
  std::vector<Vec2D> vec;
  vec.push_back (Vec2D (0, 1));
  vec.push_back (Vec2D (0, 0));
  vec.push_back (Vec2D (1, 0));

  Bezier right_triangle (vec, 0);
  for (float t = 0; t < 1.0; t += 0.1)
    cout << right_triangle.getPointAtT (t) << endl;
}

// D3 Bezier
void test4 ()
{
  /*
       * Start
       |
       |
       *------* End
              |
              |
              * End
   */
  std::vector<Vec2D> vec;
  vec.push_back (Vec2D (0, 1));
  vec.push_back (Vec2D (0, 0));
  vec.push_back (Vec2D (1, 0));
  vec.push_back (Vec2D (1, 1));

  std::cout << std::fixed;
  std::cout << std::setprecision(2);
  Bezier bolt (vec, 0);

  // not true steps, steps - 1
  for (int steps = 0; steps <= 10; steps++) {
    float t = steps / 10.0;
    cout << /* "t = " << t << ": " << */ bolt.getPointAtT (t) << endl;
  }
}


int main ()
{
  test4 ();
}