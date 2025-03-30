## General
Uses C++ to generate n-degree Bezier curves with SFML visualization.\
Calculations are done with De Casteljau's algorithm, which although naive and expensive, allows for the classic Bezier visualization.

## Controls
- Left click to add a joint
- Right click near a joint to remove it
- Drag the slider to step through time

## Build and Run
```
mkdir build
cd build
cmake ..
make
./Beziers
```

## Other Commands
In build:
```make test``` - builds and runs test.cpp\
```make run``` - builds and runs main.cpp

## Dependencies
- CMake 3.10
- SFML 2.5

## Future Plans
- Expand to 3D
- Make points draggable
