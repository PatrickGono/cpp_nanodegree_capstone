#include "controller.h"

#include "SDL.h"

auto controller::handle_input(bool &running) -> void
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) 
    {
        if (e.type == SDL_QUIT) 
        {
            running = false;
        } 
        else if (e.type == SDL_KEYDOWN) 
        {
            switch (e.key.keysym.sym) 
            {
            case SDLK_UP:
                break;
            case SDLK_DOWN:
                break;
            case SDLK_LEFT:
                break;
            case SDLK_RIGHT:
                break;
            }
        }
    }
}