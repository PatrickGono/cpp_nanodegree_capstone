#pragma once
#include "camera.h"
#include "particle.h"

#include "SDL.h"

#include <vector>

struct color
{
    constexpr color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
    color() : r(0), g(0), b(0), a(0) {}
    int r, g, b, a;
};

class renderer 
{
public: // Structors
    renderer(int screen_width, int screen_height);
    ~renderer();

public: // Interface
    auto render(const std::vector<particle>& particles, const camera& cam) -> void;
    auto update_window_title(uint64_t n_particles, double total_energy, int fps) -> void;

private: // Variables
    const std::size_t screen_width_;
    const std::size_t screen_height_;
    SDL_Window* sdl_window_;
    SDL_Renderer* sdl_renderer_;
};