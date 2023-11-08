#include "simulation.h"

constexpr double delta_t = 0.00001;
constexpr double half_delta_t_squared = 0.5 * delta_t * delta_t;
constexpr double g_const = 0.1;

simulation::simulation(uint64_t n_particles) : n_particles_{n_particles}, random_engine_{random_device_()}, random_{0, 1}
{
}

void simulation::run(renderer &renderer)
{
    particles_.emplace_back(vec(0.5, 0.5), vec(0.0, 0.0), 1000);
    for (uint64_t i = 0; i < n_particles_ - 1; ++i)
    {
        auto x = random_(random_engine_);
        auto y = random_(random_engine_);
        auto vx = 100.0 * (y - 0.5) * (0.5 + 0.5 * random_(random_engine_));
        auto vy = -100.0 * (x - 0.5) * (0.5 + 0.5 * random_(random_engine_));
        particles_.emplace_back(vec(x, y), vec(vx, vy));
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
    // 1) update positions
    for (auto& particle : particles_)
    {
        particle.pos() += particle.vel() * delta_t + particle.acc() * half_delta_t_squared;
    }

    std::vector<vec> accelerations;
    accelerations.resize(n_particles_, vec());

    // 2) calculate forces -> accelerations
    for (auto i = 0; i < particles_.size(); ++i)
    {
        auto pos_i = particles_.at(i).pos();
        auto mass_i = particles_.at(i).mass();

        for (auto j = 0; j < particles_.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }

            auto pos_j = particles_.at(j).pos();
            auto mass_j = particles_.at(j).mass();
            auto denominator = vec::distance_squared(pos_i, pos_j);
            if (denominator < 0.000001)
            {
                denominator = 100000000.0;
            }
            auto force = g_const * mass_i * mass_j / denominator * (pos_j - pos_i).normalized();
            accelerations.at(i) += force / mass_i;
            accelerations.at(j) -= force / mass_j;
        }
    }

    // 3) update velocities
    for (auto i = 0; i < particles_.size(); ++i)
    {
        particles_.at(i).vel() += 0.5 * (accelerations.at(i) + particles_.at(i).acc()) * delta_t;
        particles_.at(i).acc() = accelerations.at(i);
    }

    /*
    for (const auto& particle : particles_)
    {
        std::cout << particle;
    }
    std::cout << "===========================================\n";
    */
}
