#pragma once
#include "particle.h"
#include "SDL.h"

#include <vector>

class renderer 
{
 public:
  renderer(const size_t screen_width, const size_t screen_height,
           const size_t grid_width, const size_t grid_height);
  ~renderer();

  void render(const std::vector<particle>& particles);
  void update_window_title(uint64_t n_particles, int fps);

 private:
  SDL_Window* sdl_window_;
  SDL_Renderer* sdl_renderer_;

  const std::size_t screen_width_;
  const std::size_t screen_height_;
  const std::size_t grid_width_;
  const std::size_t grid_height_;
};