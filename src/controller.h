#pragma once
#include "simulation.h"

#include "SDL.h"

class controller 
{
public: // Interface
    static auto handle_input(simulation& sim) -> void;

private: // Implementation
    static auto handle_keyboard(const SDL_Event& event, simulation& sim) -> void;
    static auto handle_mouse(const SDL_Event& event,  simulation& sim) -> void;
};