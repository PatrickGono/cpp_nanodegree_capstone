#include "simulation.h"

#include "controller.h"
#include "tree.h"

#include <thread>

constexpr float_type delta_t = 0.00001;
constexpr float_type half_delta_t_squared = 0.5 * delta_t * delta_t;
constexpr float_type g_const = 0.1;
constexpr float_type epsilon = 0.0001;
constexpr float_type max_speed = 50.0;

///
///
simulation::simulation(uint64_t n_particles)
   : n_particles_{n_particles}
   , initial_distribution_{}
   , camera_{}
   , algorithm_{algorithm::brute_force}
   , running_{false}
   , render_quad_tree_{false}
   , frame_count_{0}
   , area_{vec(-1.0, -1.0), 2.0}
{
}

///
///
auto simulation::get_camera() -> camera&
{
    return camera_;
}

///
///
auto simulation::get_running() -> bool&
{
    return running_;
}

///
///
auto simulation::get_render_quad_tree() -> bool&
{
    return render_quad_tree_;
}

///
///
auto simulation::get_algorithm() -> algorithm&
{
    return algorithm_;
}

///
///
auto simulation::run(renderer &renderer) -> void
{
    particles_ = initial_distribution_.create_distribution(
        particle_distribution::simulation_scenario::two_clusters,
        particle_distribution::position_distribution::random_sphere,
        particle_distribution::velocity_distribution::rotating,
        n_particles_, max_speed, true);

    frame_count_ = 0;
    render_quad_tree_ = false;
    auto title_timestamp = SDL_GetTicks();
    running_ = true;

    while (running_)
    {
        controller::handle_input(*this);

        auto frame_start = SDL_GetTicks();

        update();

        if (render_quad_tree_)
        {
            // Adjust area every n frames
            if (frame_count_ % 10 == 0)
            {
                area_ = calculate_particles_bounds();
            }
        
            // Build quad tree
            auto quad_tree = tree_node(area_, nullptr);
            for (auto& particle : particles_)
            {
                quad_tree.insert_particle(&particle);
            }

            renderer.render(quad_tree, particles_, camera_);
        }
        else
        {
            renderer.render(particles_, camera_);
        }

        auto frame_end = SDL_GetTicks();
        ++frame_count_;

        if (frame_end - title_timestamp >= 1000)
        {
            float_type total_energy = 0.0; //compute_total_energy();
            renderer.update_window_title(n_particles_, total_energy, frame_count_);
            title_timestamp = frame_end;
            frame_count_ = 0;
        }
    }
}

///
///
auto simulation::update() -> void 
{
    // 1) Update positions
    for (auto& particle : particles_)
    {
        particle.pos() += particle.vel() * delta_t + particle.acc() * half_delta_t_squared;
    }

    // 2) Calculate forces -> accelerations
    std::vector<vec> accelerations;
    accelerations.resize(n_particles_, vec());
    switch (algorithm_)
    {
        case algorithm::parallel_brute_force:
        {
            calculate_brute_force_parallel(accelerations);
            break;
        }
        case algorithm::barnes_hut:
        {
            calculate_barnes_hut(accelerations);
            break;
        }
        case algorithm::brute_force:
        default:
        {
            calculate_brute_force(accelerations);
            break;
        }
    }

    // 3) Update velocities
    for (auto i = 0; i < particles_.size(); ++i)
    {
        particles_.at(i).vel() += float_type(0.5) * (accelerations.at(i) + particles_.at(i).acc()) * delta_t;
        particles_.at(i).acc() = accelerations.at(i);
    }
}

///
///
auto simulation::calculate_brute_force(std::vector<vec>& accelerations) const -> void
{
    for (auto i = 0; i < particles_.size(); ++i)
    {
        auto pos_i = particles_.at(i).pos();
        auto mass_i = particles_.at(i).mass();

        for (auto j = i + 1; j < particles_.size(); ++j)
        {
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
}

///
///
auto simulation::calculate_brute_force_parallel(std::vector<vec>& accelerations) const -> void 
{
    auto num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    auto work_chunk_size = static_cast<decltype(num_threads)>(std::ceil(n_particles_ / num_threads));

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
}

///
///
auto simulation::calculate_barnes_hut(std::vector<vec>& accelerations) -> void
{
    // Adjust area every n frames
    if (frame_count_ % 10 == 0)
    {
        area_ = calculate_particles_bounds();
    }

    // Build quad tree
    auto quad_tree = tree_node(area_, nullptr);
    for (auto& particle : particles_)
    {
        quad_tree.insert_particle(&particle);
    }

    // Compute mass statistics
    quad_tree.calculate_center_of_mass();

    // Compute accelerations
    for (auto i = 0; i < particles_.size(); ++i)
    {
        accelerations.at(i) = quad_tree.calculate_acceleration(particles_.at(i));
    }
}

///
///
auto simulation::compute_total_energy() -> float_type
{
    float_type potential_energy = 0.0;
    float_type kinetic_energy = 0.0;

    for (const auto& particle : particles_)
    {
        kinetic_energy += float_type(0.5) * particle.mass() * (particle.vel() * particle.vel());
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

///
///
auto simulation::calculate_particles_bounds() const -> square_area
{
    float_type min_x = std::numeric_limits<float_type>::max();
    float_type min_y = std::numeric_limits<float_type>::max(); 
    float_type max_x = std::numeric_limits<float_type>::min();
    float_type max_y = std::numeric_limits<float_type>::min(); 

    for (const auto& particle : particles_)
    {
        const auto& pos = particle.pos();
        min_x = std::min(min_x, pos.x());
        max_x = std::max(max_x, pos.x());
        min_y = std::min(min_y, pos.y());
        max_y = std::max(max_y, pos.y());
    }

    return square_area(vec(min_x, min_y), std::max((max_x - min_x), (max_y - min_y)));
}