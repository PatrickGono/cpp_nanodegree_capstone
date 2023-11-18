#pragma once
#include "camera.h"

#include "SDL.h"

class controller 
{
public: // Interface
    static auto handle_input(bool& running, camera& cam) -> void;

private: // Implementation
    static auto handle_keyboard(const SDL_Event& event, camera& cam) -> void;
    static auto handle_mouse(const SDL_Event& event, camera& cam) -> void;
};