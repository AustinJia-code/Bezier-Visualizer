# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(Beziers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Detect Emscripten
if(DEFINED EMSCRIPTEN)
    message(STATUS "Building with Emscripten")
    
    # Emscripten-specific flags
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    
    # Emscripten linker flags for SFML
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1")
    
    # These are required for SFML
    set(SFML_OPENGL_ES TRUE)
    add_definitions(-DSFML_OPENGL_ES)
endif()

# Add Libraries
add_library(Vec2DLib src/Vec2D.cpp)
target_include_directories(Vec2DLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_library(BezierLib src/Bezier.cpp)
target_include_directories(BezierLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_library(SliderLib src/Slider.cpp)
target_include_directories(SliderLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# SFML Configuration
if(DEFINED EMSCRIPTEN)
    # For Emscripten, use the ports system
    target_compile_options(Vec2DLib PUBLIC -sUSE_SFML=1)
    target_compile_options(BezierLib PUBLIC -sUSE_SFML=1)
    target_compile_options(SliderLib PUBLIC -sUSE_SFML=1)
    
    # Link options for Emscripten
    set(SFML_LINK_OPTIONS 
        -sUSE_SFML=1 
        -sUSE_SFML_SYSTEM=1 
        -sUSE_SFML_WINDOW=1 
        -sUSE_SFML_GRAPHICS=1
    )
else()
    # Regular SFML find_package for desktop builds
    find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)
endif()

# Add Executables
add_executable(Beziers src/main.cpp)
if(DEFINED EMSCRIPTEN)
    target_link_libraries(Beziers PRIVATE Vec2DLib BezierLib SliderLib ${SFML_LINK_OPTIONS})
    target_compile_definitions(Beziers PRIVATE EMSCRIPTEN)
else()
    target_link_libraries(Beziers PRIVATE Vec2DLib BezierLib SliderLib sfml-graphics sfml-window sfml-system)
endif()

# Test executable - only build when not using Emscripten
if(NOT DEFINED EMSCRIPTEN)
    add_executable(Test src/test.cpp)
    target_link_libraries(Test PRIVATE Vec2DLib BezierLib SliderLib sfml-graphics sfml-window sfml-system)
    
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
endif()

# "make run", builds and runs Beziers
if(DEFINED EMSCRIPTEN)
    # For Emscripten, just build the HTML
    add_custom_target(run
      COMMAND echo ""
      COMMAND echo "Build completed. Open Beziers.html in a web browser."
      COMMAND echo ""
      DEPENDS Beziers
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    # For native builds, run the executable
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
endif()