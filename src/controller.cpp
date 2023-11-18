#include "controller.h"

#include "SDL.h"

///
///
auto controller::handle_input(bool& running, camera& cam) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        if (event.type == SDL_QUIT) 
        {
            running = false;
        } 
        else if (event.type == SDL_KEYDOWN) 
        {
            switch (event.key.keysym.sym) 
            {
            case SDLK_UP:
                cam.translate_y(true);
                break;
            case SDLK_DOWN:
                cam.translate_y(false);
                break;
            case SDLK_LEFT:
                cam.translate_x(true);
                break;
            case SDLK_RIGHT:
                cam.translate_x(false);
                break;
            }
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.y > 0)
            {
                cam.zoom(true);
            }
            else if (event.wheel.y < 0)
            {
                cam.zoom(false);
            }
        }
    }
}