# N-body gravity simulation
Capstone project for the Udacity C++ Nanodegree. Simulates the gravitational attraction between simple particles in a 2D plane, and renders their motion in real time.

This Capstone project ([Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213)) is built on top of the SDL starter project provided by the course:
[CppND-Capstone-Snake-Game by Udacity](https://github.com/udacity/CppND-Capstone-Snake-Game)

The implementation of the quadtree algorithm is inspired by this work:
[Barnes-Hut-Simulator by Belt of Orion](https://github.com/beltoforion/Barnes-Hut-Simulator)

The Barnes-Hut algorithm is explained here: [Barnes-Hut simulation](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation)

## Features
The project simulates one or more clusters of particles (e.g. stars) which interact via the gravitational force. Several algorithms are implemented to calculate the forces/accelerations:
* brute force
* brute force parallelized using `std::thread`
* brute force parallelized using `std::async`
* Barnes-Hut
* Barnes-Hut parallelized using `std::thread`

In the brute force approach, each pair of particles is evaluated, leading to an O(N<sup>2</sup>) complexity (where N is the number of bodies). This can be reduced to half by considering Newton's third law. However, this still leaves the same complexity.

The Barnes-Hut algorithm relies on creating a spatial tree (quadtree in 2D, octree in 3D) of the particles, such that every particle is in its own leaf node. Each node has a center of mass and a mass corresponding to all the particles and nodes of its children. Next, a cutoff value (usually called `theta`) is chosen. If the ratio of the node size and the distance to a particle is smaller than this cutoff value, the center of mass and mass of the whole node is used instead of iterating over every particle. This allows to effectively skip evaluating particle pairs which are very far away, reducing the complexity to O(N log(N)).

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* SDL2 >= 2.0
  * All installation instructions can be found [here](https://wiki.libsdl.org/Installation)
  * Note that for Linux, an `apt` or `apt-get` installation is preferred to building from source.
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./n_body_gravity`.

## Project Specification

### README (3/3)
* A README with instructions is included with the project. ✓
* The README indicates the new features added to the project. ✓
* The README includes information about each rubric point addressed. ✓

### Compiling and Testing (1/1)
* The submission must compile and run without errors on the Udacity project workspace. ✓
  - Used the same dependencies as the start project, so should hopefully compile without any issues

### Loops, Functions, I/O (3/4)
* The project demonstrates an understanding of C++ functions and control structures. ✓ 
  - `simulation` class: `while` loop line XY, `for` loop line XY, `if` statement line XY, `switch` statement line XY
* ~~The project reads data from a file and process the data, or the program writes data to a file.~~ 𐄂
* The project accepts user input and processes the input. ✓ 
  - `controller` class
* The project uses data structures and immutable variables. ✓
  - `simulation` class uses `std::vector` on multiple occasions, as well as `const` variables whenever possible
  
### Object Oriented Programming (5/6)
* One or more classes are added to the project with appropriate access specifiers for class members. ✓
  - the whole project follows OOP, the classes `tree`, `simulation`, `particle`, `vec2` classes have accessors
* Class constructors utilize member initialization lists. ✓
  - all constructors utilize member initialization lists
* Classes abstract implementation details from their interfaces. ✓
  - `particle_distribituion` class's interface contains only one method, the rest is hidden in implementation
* Overloaded functions allow the same function to operate on different parameters. ✓
  - `renderer::render` is overloaded on lines XY and XY
* ~~Classes follow an appropriate inheritance hierarchy with virtual and override functions.~~ 𐄂
* Templates generalize functions or classes in the project.
  - `vec2` class is templated

### Memory Management (4/6)
* The project makes use of references in function declarations. ✓
  - `particle` grants access to position, velocity, and acceleration via (const) reference to avoid unnecessary copies
* ~~The project uses destructors appropriately.~~ 𐄂
  - the use of smart pointers in `tree` and `std::vector` wherever possible allows the exclusive use of default constructors
* The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate. ✓
  - all variables are initialized whenever declared
* ~~The project follows the Rule of 5.~~ 𐄂
* The project uses move semantics to move data instead of copying it, where possible. ✓
  - `tree.cpp` line 242 or `simulation.cpp` line XY
* The project uses smart pointers instead of raw pointers. ✓
  - `tree` uses smart pointers where ownership is intended, e.g. when adding child nodes to parent (`tree.h` line 52)

### Concurrency (2/4)
* The project uses multithreading. ✓
  - `simulation::calculate_brute_force_threads`, `simulation::calculate_brute_force_async`, `simulation::calculate_barnes_hut_threads`
* A promise and future is used in the project. ✓
  - `simulation::calculate_brute_force_async` line XY
* ~~A mutex or lock is used in the project.~~ 𐄂
* ~~A condition variable is used in the project.~~ 𐄂

## CC Attribution-ShareAlike 4.0 International

Shield: [![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

This work is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
