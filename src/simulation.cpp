#include "simulation.h"

#include "controller.h"
#include "tree.h"

#include <future>
#include <mutex>
#include <thread>

constexpr float_type g_const = 1;
constexpr float_type epsilon = 0.0001;
constexpr float_type max_speed = 250.0;

///
///
simulation::simulation(uint64_t n_particles)
   : n_particles_{n_particles}
   , initial_distribution_{}
   , scenario_{particle_distribution::simulation_scenario::one_cluster}
   , algorithm_{algorithm::brute_force_threads}
   , render_quad_tree_{false}
   , frame_count_{0}
   , state_{state::paused}
   , camera_{}
   , area_{vec(-1.0, -1.0), 2.0}
   , delta_t_{0.000001}
   , half_delta_t_squared_{0.5 * delta_t_ * delta_t_}
   , theta_{0.5}
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
auto simulation::get_state() -> state&
{
    return state_;
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
auto simulation::slow_down_simulation() -> void 
{
    delta_t_ *= 0.9;
    half_delta_t_squared_ = 0.5 * delta_t_ * delta_t_;
}

///
///
auto simulation::reverse_simulation() -> void
{
    delta_t_ = -delta_t_;
}

///
///
auto simulation::speed_up_simulation() -> void 
{
    delta_t_ *= 1.1;
    half_delta_t_squared_ = 0.5 * delta_t_ * delta_t_;
}

///
///
auto simulation::set_scenario(particle_distribution::simulation_scenario scenario) -> void
{
    scenario_ = scenario;
    init();
}

///
///
auto simulation::increase_particles_by_1000_and_restart() -> void
{
    n_particles_ += 1000;
    init();
}

///
///
auto simulation::decrease_particles_by_1000_and_restart() -> void
{
    if (n_particles_ > 1000)
    {
        n_particles_ -= 1000;
    }
    init();
}

///
///
auto simulation::increase_theta() -> void
{
    theta_ = std::min(1.0, theta_ + 0.1);
}

///
///
auto simulation::decrease_theta() -> void
{
    theta_ = std::max(0.0, theta_ - 0.1);
}

///
///
auto simulation::run(renderer &renderer) -> void
{
    init();
    auto title_timestamp = SDL_GetTicks();

    while (state_ != state::exiting)
    {
        controller::handle_input(*this);
        auto frame_start = SDL_GetTicks();

        // Update simulation or wait
        if (state_ == state::running)
        {
            update();
        }
        else if (state_ == state::paused)
        {
            SDL_Delay(16);
        }

        // Render
        if (render_quad_tree_)
        {
            const auto quad_tree = create_quad_tree();
            renderer.render(quad_tree, particles_, camera_);
        }
        else
        {
            renderer.render(particles_, camera_);
        }

        // Post-update
        auto frame_end = SDL_GetTicks();
        ++frame_count_;

        if (frame_end - title_timestamp >= 1000)
        {
            renderer.update_window_title(n_particles_, frame_count_);
            title_timestamp = frame_end;
            frame_count_ = 0;
        }
    }
}

///
///
auto simulation::init() -> void
{
    particles_ = initial_distribution_.create_distribution(
        scenario_,
        particle_distribution::position_distribution::random_sphere,
        particle_distribution::velocity_distribution::rotating,
        n_particles_, max_speed, true);

    frame_count_ = 0;
    render_quad_tree_ = false;
    state_ = state::running;
}

///
///
auto simulation::update() -> void 
{
    // 1) Update positions
    for (auto& particle : particles_)
    {
        particle.pos() += particle.vel() * delta_t_ + particle.acc() * half_delta_t_squared_;
    }

    // 2) Calculate forces -> accelerations
    std::vector<vec> accelerations;
    accelerations.resize(n_particles_, vec());
    switch (algorithm_)
    {
        case algorithm::brute_force_threads:
        {
            calculate_brute_force_threads(accelerations);
            break;
        }
        case algorithm::brute_force_async:
        {
            calculate_brute_force_async(accelerations);
            break;
        }
        case algorithm::barnes_hut:
        {
            calculate_barnes_hut(accelerations);
            break;
        }
        case algorithm::barnes_hut_threads:
        {
            calculate_barnes_hut_threads(accelerations);
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
        particles_.at(i).vel() += float_type(0.5) * (accelerations.at(i) + particles_.at(i).acc()) * delta_t_;
        particles_.at(i).acc() = accelerations.at(i);
    }
}

///
///
auto simulation::calculate_brute_force(std::vector<vec>& accelerations) const -> void
{
    for (auto i = 0; i < particles_.size(); ++i)
    {
        const auto& pos_i = particles_.at(i).pos();
        const auto& mass_i = particles_.at(i).mass();

        for (auto j = i + 1; j < particles_.size(); ++j)
        {
            const auto& pos_j = particles_.at(j).pos();
            const auto& mass_j = particles_.at(j).mass();
            auto denominator = vec::distance_squared(pos_i, pos_j);
            if (denominator < epsilon)
            {
                denominator = epsilon;
            }
            const auto force = g_const * mass_i * mass_j / denominator * (pos_j - pos_i).normalized();
            accelerations.at(i) += force / mass_i;
            accelerations.at(j) -= force / mass_j;
        }
    }
}

///
///
auto simulation::calculate_brute_force_threads(std::vector<vec>& accelerations) const -> void 
{
    // Debug output to figure out best choice of thread count
    const auto debug_output = false;
    std::mutex cout_mutex;

    std::vector<std::thread> threads;
    const auto num_threads = std::thread::hardware_concurrency() / 2;
    const auto work_chunk_size = static_cast<decltype(num_threads)>(std::ceil(n_particles_ / num_threads));

    auto compute_accelerations = [&accelerations, &cout_mutex, debug_output, this](size_t chunk_start, size_t chunk_end) {
        const auto start_time = SDL_GetTicks();

        for (auto i = chunk_start; i < chunk_end; ++i)
        {
            const auto& pos_i = particles_.at(i).pos();
            const auto& mass_i = particles_.at(i).mass();

            auto acceleration = vec();

            for (auto j = 0; j < particles_.size(); ++j)
            {
                if (i == j)
                {
                    continue;
                }

                const auto& pos_j = particles_.at(j).pos();
                const auto& mass_j = particles_.at(j).mass();
                auto denominator = vec::distance_squared(pos_i, pos_j);
                if (denominator < epsilon)
                {
                    denominator = epsilon;
                }
                acceleration += g_const * mass_j / denominator * (pos_j - pos_i).normalized();
            }
            accelerations.at(i) += acceleration;
        }

        const auto end_time = SDL_GetTicks();
        if (debug_output)
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "thread " << std::this_thread::get_id() << " finished in: " << end_time - start_time << "ms\n";
        }
    };

    for (auto thread_index = 0u; thread_index < num_threads; thread_index++)
    {
        const auto chunk_start = thread_index * work_chunk_size;
        const auto chunk_end = std::min(static_cast<unsigned int>(n_particles_), (thread_index + 1) * work_chunk_size);
        threads.emplace_back(compute_accelerations, chunk_start, chunk_end);   
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

///
///
auto simulation::calculate_brute_force_async(std::vector<vec>& accelerations) const -> void 
{
    std::vector<std::future<void>> futures;
    const auto num_threads = std::thread::hardware_concurrency() / 2;
    const auto work_chunk_size = static_cast<decltype(num_threads)>(std::ceil(n_particles_ / num_threads));

    auto compute_accelerations = [&accelerations, this](size_t chunk_start, size_t chunk_end) {
        for (auto i = chunk_start; i < chunk_end; ++i)
        {
            const auto& pos_i = particles_.at(i).pos();
            const auto& mass_i = particles_.at(i).mass();

            auto acceleration = vec();

            for (auto j = 0; j < particles_.size(); ++j)
            {
                if (i == j)
                {
                    continue;
                }

                const auto& pos_j = particles_.at(j).pos();
                const auto& mass_j = particles_.at(j).mass();
                auto denominator = vec::distance_squared(pos_i, pos_j);
                if (denominator < epsilon)
                {
                    denominator = epsilon;
                }
                acceleration += g_const * mass_j / denominator * (pos_j - pos_i).normalized();
            }
            accelerations.at(i) += acceleration;
        }
    };

    for (auto thread_index = 0u; thread_index < num_threads; thread_index++)
    {
        const auto chunk_start = thread_index * work_chunk_size;
        const auto chunk_end = std::min(static_cast<unsigned int>(n_particles_), (thread_index + 1) * work_chunk_size);
        auto future = std::async(std::launch::async, compute_accelerations, chunk_start, chunk_end);
        futures.emplace_back(std::move(future));
    }

    for (auto& future : futures)
    {
        future.get();
    }
}

///
///
auto simulation::calculate_barnes_hut(std::vector<vec>& accelerations) -> void
{
    // Build quad tree
    auto quad_tree = create_quad_tree();

    // Compute mass statistics
    quad_tree.calculate_center_of_mass();

    // Compute accelerations
    for (auto i = 0; i < particles_.size(); ++i)
    {
        accelerations.at(i) = quad_tree.calculate_acceleration(particles_.at(i), theta_);
    }
}

///
///
auto simulation::calculate_barnes_hut_threads(std::vector<vec>& accelerations) -> void
{
    // Build quad tree
    auto quad_tree = create_quad_tree();

    // Compute mass statistics
    quad_tree.calculate_center_of_mass();

    // Define thread function
    std::vector<std::thread> threads;
    const auto num_threads = std::thread::hardware_concurrency() / 2;
    const auto work_chunk_size = static_cast<decltype(num_threads)>(std::ceil(n_particles_ / num_threads));

    auto compute_accelerations = [&accelerations, this, &quad_tree](size_t chunk_start, size_t chunk_end) {
        for (auto i = chunk_start; i < chunk_end; ++i)
        {
            const auto acceleration = quad_tree.calculate_acceleration(particles_.at(i), theta_);
            accelerations.at(i) += acceleration;
        }
    };

    // Create thread objects
    for (auto thread_index = 0u; thread_index < num_threads; thread_index++)
    {
        const auto chunk_start = thread_index * work_chunk_size;
        const auto chunk_end = std::min(static_cast<unsigned int>(n_particles_), (thread_index + 1) * work_chunk_size);
        threads.emplace_back(compute_accelerations, chunk_start, chunk_end);   
    }

    // Compute accelerations 
    for (auto& thread : threads)
    {
        thread.join();
    }
}

///
///
auto simulation::create_quad_tree() -> tree_node
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

    return quad_tree;
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