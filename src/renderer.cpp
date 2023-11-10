#include "renderer.h"
#include <iostream>
#include <string>
#include <vector>

renderer::renderer(const std::size_t screen_width, const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width_(screen_width)
    , screen_height_(screen_height)
    , grid_width_(grid_width)
    , grid_height_(grid_height) 
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "SDL could not initialize.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }

    // Create Window
    sdl_window_ = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, screen_width,
                                    screen_height, SDL_WINDOW_SHOWN);

    if (sdl_window_ == nullptr) 
    {
        std::cerr << "Window could not be created.\n";
        std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
    }

    // Create renderer
    sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer_ == nullptr) 
    {
        std::cerr << "Renderer could not be created.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }
}

renderer::~renderer() 
{
    SDL_DestroyWindow(sdl_window_);
    SDL_Quit();
}

void renderer::update_window_title(uint64_t n_particles, double total_energy, int fps) 
{
    std::string title{"Particles: " + std::to_string(n_particles) + " Energy: " + std::to_string(total_energy) + " FPS: " + std::to_string(fps)};
    SDL_SetWindowTitle(sdl_window_, title.c_str());
}

void renderer::render(const std::vector<particle>& particles)
{
    std::vector<SDL_Point> points;
    for (const auto& particle : particles)
    {
        SDL_Point point;
        point.x = screen_width_ * particle.pos().x();
        point.y = screen_height_ * particle.pos().y();
        points.push_back(point);
    }
  
    // Clear screen
    SDL_SetRenderDrawColor(sdl_renderer_, 0x1E, 0x1E, 0x1E, 0xFF);
    SDL_RenderClear(sdl_renderer_);
  
    // Render particles
    SDL_SetRenderDrawColor(sdl_renderer_, 0xFF, 0xCC, 0x00, 0xFF);
    SDL_RenderDrawPoints(sdl_renderer_, points.data(), points.size());
  
    // Update Screen
    SDL_RenderPresent(sdl_renderer_);
}
