# General
Uses SFML and C++ to generate and display n-degree Bezier curves.
Calculations are done with De Casteljau's algorithm, which although naive and
expensive, allows for the classic Bezier visualization.

# Build
mkdir build
cd build
cmake ..
make
./Beziers

# Controls
Left click to add a joint
Right click near a joint to remove it
Drag the slider to step through time

# Other Commands
make test, builds and runs test.cpp
make run, builds and runs main.cpp

# Dependencies
CMake
SFML 3.0