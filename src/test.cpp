#include "Vec2D.hpp"
using namespace std;

int main() {
  Vec2D point (0, 2);
  cout << "Point: " << point << endl;
  Vec2D other (0, 4);
  cout << other + point << endl;
  cout << other * 3 << endl;
  cout << Vec2D::lerp (point, other, 0.2) << endl;
}