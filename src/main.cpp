#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "renderer.h"
#include "simulation.h"

int main(int argc, char* argv[]) 
{
  constexpr int screen_width{1024};
  constexpr int screen_height{1024};

  simulation sim(10000);
  renderer renderer(screen_width, screen_height);
  sim.run(renderer);

  return 0;
}