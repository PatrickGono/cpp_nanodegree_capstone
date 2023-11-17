#include "simulation.h"

#include "controller.h"
#include "tree.h"

#include <thread>

constexpr double delta_t = 0.00001;
constexpr double half_delta_t_squared = 0.5 * delta_t * delta_t;
constexpr double g_const = 0.1;
constexpr double epsilon = 0.0001;
constexpr double max_speed = 50.0;

///
///
simulation::simulation(uint64_t n_particles) : n_particles_{n_particles}, initial_distribution_{}
{
}

///
///
auto simulation::run(renderer &renderer) -> void
{
    particles_ = initial_distribution_.create_distribution(particle_distribution::position_distribution::random_sphere,
                                                           particle_distribution::velocity_distribution::rotating,
                                                           n_particles_, max_speed, true, 0.1);

    auto frame_count = 0;
    auto title_timestamp = SDL_GetTicks();
    auto running = true;

    while (running)
    {
        controller::handle_input(running);

        auto frame_start = SDL_GetTicks();

        update_barnes_hut();
        renderer.render(particles_);

        auto frame_end = SDL_GetTicks();
        ++frame_count;

        if (frame_end - title_timestamp >= 1000)
        {
            auto total_energy = 0.0; //compute_total_energy();
            renderer.update_window_title(n_particles_, total_energy, frame_count);
            title_timestamp = frame_end;
            frame_count = 0;
        }
    }
}

///
///
auto simulation::update() -> void 
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

        for (auto j = i + 1; j < particles_.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }

            auto pos_j = particles_.at(j).pos();
            auto mass_j = particles_.at(j).mass();
            auto denominator = vec::distance_squared(pos_i, pos_j);
            if (denominator < epsilon)
            {
                denominator = epsilon;
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
}

///
///
auto simulation::update_thread() -> void 
{
    // 1) update positions
    for (auto& particle : particles_)
    {
        particle.pos() += particle.vel() * delta_t + particle.acc() * half_delta_t_squared;
    }

    std::vector<vec> accelerations;
    accelerations.resize(n_particles_, vec());

    auto num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    auto work_chunk_size = static_cast<decltype(num_threads)>(std::ceil(n_particles_ / num_threads));

    // 2) calculate forces -> accelerations
    auto compute_forces = [&accelerations, this](size_t chunk_start, size_t chunk_end) {
        for (auto i = chunk_start; i < chunk_end; ++i)
        {
            auto pos_i = particles_.at(i).pos();
            auto mass_i = particles_.at(i).mass();

            auto force = vec();

            for (auto j = 0; j < particles_.size(); ++j)
            {
                if (i == j)
                {
                    continue;
                }

                auto pos_j = particles_.at(j).pos();
                auto mass_j = particles_.at(j).mass();
                auto denominator = vec::distance_squared(pos_i, pos_j);
                if (denominator < epsilon)
                {
                    denominator = epsilon;
                }
                force += g_const * mass_i * mass_j / denominator * (pos_j - pos_i).normalized();
            }
            accelerations.at(i) += force / mass_i;
        }
    };

    for (auto thread_index = 0u; thread_index < num_threads; thread_index++)
    {
        auto chunk_start = thread_index * work_chunk_size;
        auto chunk_end = std::min(static_cast<unsigned int>(n_particles_), (thread_index + 1) * work_chunk_size);
        threads.emplace_back(compute_forces, chunk_start, chunk_end);   
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    // 3) update velocities
    for (auto i = 0; i < particles_.size(); ++i)
    {
        particles_.at(i).vel() += 0.5 * (accelerations.at(i) + particles_.at(i).acc()) * delta_t;
        particles_.at(i).acc() = accelerations.at(i);
    }
}

///
///
auto simulation::update_barnes_hut() -> void
{
    // 1) update positions
    for (auto& particle : particles_)
    {
        particle.pos() += particle.vel() * delta_t + particle.acc() * half_delta_t_squared;
    }

    std::vector<vec> accelerations;
    accelerations.resize(n_particles_, vec());

    // build tree
    square_area area;
    area.side = 3.0;
    area.top_left_corner = vec(-1, -1);
    auto quad_tree = tree_node(area, nullptr);
    for (auto& particle : particles_)
    {
        quad_tree.insert_particle(&particle);
    }

    // compute mass statistics
    quad_tree.calculate_center_of_mass();

    // 2) calculate forces -> accelerations
    for (auto i = 0; i < particles_.size(); ++i)
    {
        accelerations.at(i) = quad_tree.calculate_acceleration(particles_.at(i));
    }

    // 3) update velocities
    for (auto i = 0; i < particles_.size(); ++i)
    {
        particles_.at(i).vel() += 0.5 * (accelerations.at(i) + particles_.at(i).acc()) * delta_t;
        particles_.at(i).acc() = accelerations.at(i);
    }
}

///
///
auto simulation::compute_total_energy() -> double
{
    auto potential_energy = 0.0;
    auto kinetic_energy = 0.0;

    for (const auto& particle : particles_)
    {
        kinetic_energy += 0.5 * particle.mass() * (particle.vel() * particle.vel());
        for (const auto& other_particle : particles_)
        {
            if (&other_particle == &particle)
            {
                continue;
            }
            potential_energy += -g_const * particle.mass() * other_particle.mass() / vec::distance(particle.pos(), other_particle.pos());
        }
    }
    return (potential_energy + kinetic_energy) / 1000000;
}
