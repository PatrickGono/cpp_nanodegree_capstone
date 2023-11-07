#include <iostream>
#include "controller.h"
#include "renderer.h"
#include "vec2.h"
#include "particle.h"
#include "simulation.h"

int main(int argc, char *argv[]) 
{
  constexpr std::size_t frames_per_second{60};
  constexpr std::size_t ms_per_frame{1000 / frames_per_second};
  constexpr std::size_t screen_width{640};
  constexpr std::size_t screen_height{640};
  constexpr std::size_t grid_width{32};
  constexpr std::size_t grid_height{32};

  simulation sim(30000);
  renderer renderer(screen_width, screen_height, grid_width, grid_height);
  sim.run(renderer);

  return 0;
}