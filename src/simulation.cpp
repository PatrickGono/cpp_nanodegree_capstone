#include "simulation.h"

simulation::simulation(uint64_t n_particles) : n_particles_{n_particles}, random_engine_{random_device_()}, random_{0, 1}
{
}

void simulation::run(renderer &renderer)
{
    for (uint64_t i = 0; i < n_particles_; ++i)
    {
        auto x = random_(random_engine_);
        auto y = random_(random_engine_);
        particles_.emplace_back(vec(x, y));
    }

    auto frame_count = 0;
    auto title_timestamp = SDL_GetTicks();

    while(true)
    {
        auto frame_start = SDL_GetTicks();

        update();
        renderer.render(particles_);

        auto frame_end = SDL_GetTicks();
        ++frame_count;

        if (frame_end - title_timestamp >= 1000)
        {
            renderer.update_window_title(n_particles_, frame_count);
            title_timestamp = frame_end;
            frame_count = 0;
        }
    }
}

void simulation::update()
{
    for (auto& particle : particles_)
    {
        auto x = random_(random_engine_);
        auto y = random_(random_engine_);
        particle.pos() = vec(x, y);
    }
}
