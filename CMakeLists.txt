# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(Beziers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Libraries
add_library(Vec2DLib src/Vec2D.cpp)
target_include_directories(Vec2DLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Add Executables
add_executable(Beziers src/main.cpp)
target_link_libraries(Beziers PRIVATE Vec2DLib)
add_executable(Test src/test.cpp)
target_link_libraries(Test PRIVATE Vec2DLib)

# "make run", builds and runs Beziers
add_custom_target(run
  COMMAND echo ""
  COMMAND echo "Running Beziers"
  COMMAND echo ""
  COMMAND ./Beziers
  COMMAND echo ""
  COMMAND echo "Finished Beziers"
  COMMAND echo ""
  DEPENDS Beziers
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# "make test", builds and runs Tests
add_custom_target(test
  COMMAND echo ""
  COMMAND echo "Running Test"
  COMMAND echo ""
  COMMAND Test
  COMMAND echo ""
  COMMAND echo "Finished Test"
  COMMAND echo ""
  DEPENDS Test
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)