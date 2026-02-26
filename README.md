## Python (main)
3D drone path-following simulation using Bezier splines and RRT* planning.

### Structure
```
main.py          # entry point
src/
  geometry.py    # Vec3D, KDTree, Locatable
  waypoint.py    # Waypoint, WaypointPath
  obstacle.py    # Obstacle (sphere)
  bezier.py      # BezierCurve, BezierSpline
  RRT.py         # RRT* path planner
  drone.py       # Drone simulation, PID controller
```

### Run
```
python main.py [scene]
```

### Dependencies
- matplotlib
- numpy

---

## C++ Basic Beziers (`basic/`)
Generates n-degree Bezier curves with SFML visualization.\
Calculations use De Casteljau's algorithm with an interactive joint editor.

### Controls
- Left click to add a joint
- Right click near a joint to remove it
- Drag the slider to step through time

### Build and Run
```
cd basic
mkdir build
cd build
cmake ..
make
./Beziers
```

### Other Commands
In `basic/build`:
```make test``` - builds and runs test.cpp\
```make run``` - builds and runs main.cpp

### Dependencies
- CMake 3.10
- SFML 2.5
