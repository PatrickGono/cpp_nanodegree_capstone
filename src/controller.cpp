#include "controller.h"

#include "simulation.h"

#include "SDL.h"

///
///
auto controller::handle_input(simulation& sim) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        if (event.type == SDL_QUIT) 
        {
            sim.get_running() = false;
        } 
        handle_keyboard(event, sim);
        handle_mouse(event, sim);
    }
}

///
///
auto controller::handle_keyboard(const SDL_Event& event, simulation& sim) -> void
{
    if (event.type == SDL_KEYDOWN) 
    {
        switch (event.key.keysym.sym) 
        {
            // Camera controls
            case SDLK_UP:
            case SDLK_w:
            {
                sim.get_camera().translate_y(true);
                break;
            }
            case SDLK_DOWN:
            case SDLK_s:
            {
                sim.get_camera().translate_y(false);
                break;
            }
            case SDLK_LEFT:
            case SDLK_a:
            {
                sim.get_camera().translate_x(true);
                break;
            }
            case SDLK_RIGHT:
            case SDLK_d:
            {
                sim.get_camera().translate_x(false);
                break;
            }
            // Algorithm selection
            case SDLK_1:
            {
                sim.get_algorithm() = simulation::algorithm::brute_force;
                break;
            }
            case SDLK_2:
            {
                sim.get_algorithm() = simulation::algorithm::brute_force_threads;
                break;
            }
            case SDLK_3:
            {
                sim.get_algorithm() = simulation::algorithm::brute_force_async;
                break;
            }
            case SDLK_4:
            {
                sim.get_algorithm() = simulation::algorithm::barnes_hut;
                break;
            }
            case SDLK_5:
            {
                sim.get_algorithm() = simulation::algorithm::barnes_hut_threads;
                break;
            }
            // Rendering options
            case SDLK_q:
            {
                sim.get_render_quad_tree() = !sim.get_render_quad_tree();
                break;
            }
            // Simulation modifiers
            case SDLK_e:
            {
                sim.slow_down_simulation();
                break;
            }
            case SDLK_r:
            {
                sim.reverse_simulation();
                break;
            }
            case SDLK_t:
            {
                sim.speed_up_simulation();
                break;
            }
            case SDLK_i:
            {
                sim.set_scenario(particle_distribution::simulation_scenario::one_cluster);
                break;
            }
            case SDLK_o:
            {
                sim.set_scenario(particle_distribution::simulation_scenario::two_clusters);
                break;
            }
            case SDLK_p:
            {
                sim.set_scenario(particle_distribution::simulation_scenario::cluster_and_black_hole);
                break;
            }
            default:
            {
                // Do nothing
                break;
            }
        }
    }
}

///
///
auto controller::handle_mouse(const SDL_Event& event, simulation& sim) -> void
{
    if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y > 0)
        {
            sim.get_camera().zoom(true);
        }
        else if (event.wheel.y < 0)
        {
            sim.get_camera().zoom(false);
        }
    }
}