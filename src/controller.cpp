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
        handle_keyboard(event, cam);
        handle_mouse(event, cam);
    }
}


///
///
auto controller::handle_keyboard(const SDL_Event& event, camera& cam) -> void
{
    if (event.type == SDL_KEYDOWN) 
    {
        switch (event.key.keysym.sym) 
        {
        case SDLK_UP:
        case SDLK_w:
            cam.translate_y(true);
            break;
        case SDLK_DOWN:
        case SDLK_s:
            cam.translate_y(false);
            break;
        case SDLK_LEFT:
        case SDLK_a:
            cam.translate_x(true);
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            cam.translate_x(false);
            break;
        }
    }
}

///
///
auto controller::handle_mouse(const SDL_Event& event, camera& cam) -> void
{
    if (event.type == SDL_MOUSEWHEEL)
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