#include "particle_distribution.h"

#include "vec2.h"

#include <math.h>
#include <numeric>
#include <random>

constexpr float_type central_body_mass = 1000.0;
constexpr float_type gamma = 0.25;
constexpr float_type pi = 3.14159265358979311600;
constexpr float_type g_const = 1.0;

///
///
particle_distribution::particle_distribution() : random_engine_{random_device_()}, random_uniform_{0, 1}, random_cauchy_{0, gamma}
{
}

///
///
auto particle_distribution::create_distribution(
    simulation_scenario scenario,
    position_distribution pos_dist, 
    velocity_distribution vel_dist, 
    uint64_t n_particles,
    float_type max_speed, 
    bool add_central_body /* = true */) -> std::vector<particle>
{
    std::vector<particle> particles;
    particles.reserve(n_particles);

    switch (scenario)
    {
        case simulation_scenario::cluster_and_black_hole:
        {
            // Create cluster.
            const auto cluster_n_particles = n_particles - 1;
            const auto center = vec(-0.3, -0.3);
            const auto velocity = vec(0.3 * max_speed, 0.2 * max_speed);
            const auto radius = 0.25;

            const auto cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, cluster_n_particles, max_speed, add_central_body);
            particles.insert(particles.begin(), cluster.begin(), cluster.end());

            // Add black hole.
            const auto black_hole_position = vec(0.3, 0.3);
            const auto black_hole_velocity = vec(-0.3 * max_speed, -0.2 * max_speed);
            const auto black_hole_mass = 2000;
            particles.emplace_back(black_hole_position, black_hole_velocity, black_hole_mass);
            break;
        }
        case simulation_scenario::two_clusters:
        {
            // Create first cluster.
            const auto first_n_particles = n_particles / 2;
            auto center = vec(-0.3, -0.3);
            auto velocity = vec(0.3 * max_speed, 0.2 * max_speed);
            auto radius = 0.25;
            const auto first_cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, first_n_particles, max_speed, add_central_body);
    
            // Create second cluster.
            const auto second_n_particles = n_particles - first_n_particles;
            center = vec(0.3, 0.3);
            velocity = vec(-0.3 * max_speed, -0.2 * max_speed);
            radius = 0.25;
            const auto second_cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, second_n_particles, max_speed, add_central_body);
    
            // Concatenate particles.
            particles.insert(particles.begin(), first_cluster.begin(), first_cluster.end());
            particles.insert(particles.end(), second_cluster.begin(), second_cluster.end());
            break;
        }
        case simulation_scenario::one_cluster:
        default:
        {
            create_position_distribution(pos_dist, particles, n_particles, add_central_body);
            create_velocity_distribution(vel_dist, particles, max_speed);
            break;
        }
    }

    return particles;
}

///
///
auto particle_distribution::create_position_distribution(
    position_distribution pos_dist, 
    std::vector<particle>& particles,
    uint64_t n_particles,
    bool add_central_body) -> void
{
    // Create optional central body (~ black hole).
    size_t particle_index = 0;
    if (add_central_body)
    {
        particles.emplace_back(vec(0.0, 0.0), vec(0.0, 0.0), central_body_mass);
        ++particle_index;
    }

    // Create particles with the desired position distribution.
    for (; particle_index < n_particles; ++particle_index)
    {
        switch (pos_dist)
        {
            case position_distribution::galaxy:
            {
                particles.emplace_back(generate_random_vec_galaxy(), vec(), 1.0);
                break;
            }
            case position_distribution::random_square:
            {
                particles.emplace_back(generate_random_vec_uniform() - vec(0.5), vec(), 1.0);
                break;
            }
            case position_distribution::random_sphere:
            default:
            {
                while (true)
                {
                    auto random_vec = generate_random_vec_uniform() - vec(0.5);
                    if (vec::distance(random_vec, vec(0.0)) < 0.5)
                    {
                        particles.emplace_back(random_vec, vec(), 1.0);
                        break;
                    }
                }
                break;
            }
        }
    }
}

///
///
auto particle_distribution::create_velocity_distribution(
    velocity_distribution vel_dist,
    std::vector<particle>& particles,
    float_type max_speed) -> void
{
    // Sort particles by distance from center in case of galaxy and calculate masses.
    auto masses_of_particles_closer_to_center = std::vector<float_type>{};
    if (vel_dist == velocity_distribution::galaxy)
    {
        std::sort(particles.begin(), particles.end(), [](const auto& first, const auto& second)
        {
            return first.pos().length_squared() < second.pos().length_squared();
        });

        masses_of_particles_closer_to_center.reserve(particles.size());
        for (const auto& part : particles)
        {
            masses_of_particles_closer_to_center.push_back(part.mass());
        }

        std::partial_sum(
            masses_of_particles_closer_to_center.begin(), 
            masses_of_particles_closer_to_center.end(), 
            masses_of_particles_closer_to_center.begin());
    }

    // Adjust particle velocities according to the desired velocity distribution.
    for (auto iter = particles.begin(); iter < particles.end(); ++iter)
    {
        switch (vel_dist)
        {
            case velocity_distribution::random:
            {
                auto velocity_direction = (generate_random_vec_uniform() - vec(0.5)).normalized();
                auto speed = max_speed * random_uniform_(random_engine_);
                iter->vel() = speed * velocity_direction;
                break;
            }
            case velocity_distribution::galaxy:
            {
                // Calculates orbital velocities using the 2D shell theorem for gravity.
                const auto index = std::distance(particles.begin(), iter);
                const auto dist = iter->pos().length();
                if (dist <= 0.00001)
                {
                    iter->vel() = vec(0);
                    break;
                }
                const auto mass_inside_shell = masses_of_particles_closer_to_center.at(index);
                auto speed = std::sqrt(g_const * mass_inside_shell / dist);
                // Slow down particles near the center with heuristic factor.
                speed *= dist / (dist + 0.005);
                const auto vx = iter->pos().y();
                const auto vy = -iter->pos().x();
                const auto velocity_direction = vec(vx, vy).normalized();
                iter->vel() = speed * velocity_direction;
                break;
            }
            case velocity_distribution::rotating:
            default:
            {
                auto vx = max_speed * iter->pos().y() * 2.0;
                auto vy = -max_speed * iter->pos().x() * 2.0;
                iter->vel() = vec(vx, vy);
                break;
            }
        }
    }
}

///
///
auto particle_distribution::create_cluster(
    vec center,
    vec velocity,
    float_type radius,
    position_distribution pos_dist,
    velocity_distribution vel_dist,
    uint64_t n_particles,
    float_type max_speed,
    bool add_central_body) -> std::vector<particle>
{
    std::vector<particle> particles;
    particles.reserve(n_particles);
    create_position_distribution(pos_dist, particles, n_particles, add_central_body);
    create_velocity_distribution(vel_dist, particles, max_speed);
    
    // Scale and shift cluster.
    for (auto& part : particles)
    {
        part.pos() *= 2 * radius;
        part.pos() += center;
        part.vel() += velocity;
    }

    return particles;
}

///
///
auto particle_distribution::generate_random_vec_uniform() -> vec
{
    auto x = random_uniform_(random_engine_);
    auto y = random_uniform_(random_engine_);
    return vec(x, y);
}

///
///
auto particle_distribution::generate_random_vec_galaxy() -> vec
{
    auto distance = 2.0;
    auto vector = vec();
    while (distance > 1.0)
    {
        const auto ur = random_cauchy_(random_engine_);
        distance = std::abs(ur);
        const auto ut = random_uniform_(random_engine_);
        const auto x = 0.5 * ur * std::cos(2 * pi * ut);
        const auto y = 0.5 * ur * std::sin(2 * pi * ut);
        vector = vec(x, y);
    }
    return vector;
}