#include "particle_distribution.h"

#include "vec2.h"

#include <random>

constexpr float_type central_body_mass = 1000.0;

///
///
particle_distribution::particle_distribution() : random_engine_{random_device_()}, random_{0, 1}
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
    bool add_central_body /* = true */,
    float_type randomization_ratio /* = 0.0 */) -> std::vector<particle>
{
    std::vector<particle> particles;
    particles.reserve(n_particles);

    switch (scenario)
    {
        case simulation_scenario::cluster_and_black_hole:
        {
            // Create cluster
            auto cluster_n_particles = n_particles - 1;
            auto center = vec(-0.3, -0.3);
            auto velocity = vec(20, 12);
            auto radius = 0.25;

            auto cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, cluster_n_particles, max_speed, add_central_body, randomization_ratio);
            particles.insert(particles.begin(), cluster.begin(), cluster.end());

            // Add black hole
            const auto black_hole_position = vec(0.3, 0.3);
            const auto black_hole_velocity = vec(-20, -12);
            const auto black_hole_mass = 2000;
            particles.emplace_back(black_hole_position, black_hole_velocity, black_hole_mass);
            break;
        }
        case simulation_scenario::two_clusters:
        {
            // Create first cluster
            auto first_n_particles = n_particles / 2;
            auto center = vec(-0.3, -0.3);
            auto velocity = vec(20, 12);
            auto radius = 0.25;
            auto first_cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, first_n_particles, max_speed, add_central_body, randomization_ratio);
    
            // Create second cluster
            auto second_n_particles = n_particles - first_n_particles;
            center = vec(0.3, 0.3);
            velocity = vec(-20, -12);
            radius = 0.25;
            auto second_cluster = create_cluster(center, velocity, radius, pos_dist, vel_dist, second_n_particles, max_speed, add_central_body, randomization_ratio);
    
            // Concatenate particles
            particles.insert(particles.begin(), first_cluster.begin(), first_cluster.end());
            particles.insert(particles.end(), second_cluster.begin(), second_cluster.end());
            break;
        }
        case simulation_scenario::one_cluster:
        default:
        {
            create_position_distribution(pos_dist, particles, n_particles, add_central_body);
            create_velocity_distribution(vel_dist, particles, max_speed, randomization_ratio);
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
    // create optional central body (~ black hole)
    size_t particle_index = 0;
    if (add_central_body)
    {
        particles.emplace_back(vec(0.0, 0.0), vec(0.0, 0.0), central_body_mass);
        ++particle_index;
    }

    // create particles with the desired position distribution
    for (; particle_index < n_particles; ++particle_index)
    {
        switch (pos_dist)
        {
            case position_distribution::galaxy:
            {
                // TODO implement some sort of galactic distribution
                // fallthrough for now!
            }
            case position_distribution::random_square:
            {
                particles.emplace_back(generate_random_vec() - vec(0.5), vec(), 1.0);
                break;
            }
            case position_distribution::random_sphere:
            default:
            {
                while (true)
                {
                    auto random_vec = generate_random_vec() - vec(0.5);
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
    float_type max_speed,
    float_type randomization_ratio) -> void
{
    // adjust particle velocities according to the desired velocity distribution
    for (auto& part: particles)
    {
        switch (vel_dist)
        {
            case velocity_distribution::random:
            {
                auto velocity_direction = (generate_random_vec() - vec(0.5)).normalized();
                auto speed = max_speed * random_(random_engine_);
                part.vel() = speed * velocity_direction;
                break;
            }
            case velocity_distribution::galaxy:
            {
                // TODO generate galactic velocity distribution
                // fallthrough for now!
            }
            case velocity_distribution::rotating:
            default:
            {
                auto random_direction = generate_random_vec().normalized();
                auto random_factor = random_(random_engine_);
                auto minimum_speed = (1 - randomization_ratio) * max_speed;
                auto random_speed = random_factor * (max_speed - minimum_speed);
                auto vx = minimum_speed * part.pos().y() * 2.0;
                auto vy = -minimum_speed * part.pos().x() * 2.0;
                part.vel() = vec(vx, vy) + random_speed * random_direction;
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
    bool add_central_body,
    float_type randomization_ratio) -> std::vector<particle>
{
    std::vector<particle> particles;
    particles.reserve(n_particles);
    create_position_distribution(pos_dist, particles, n_particles, add_central_body);
    create_velocity_distribution(vel_dist, particles, max_speed, randomization_ratio);
    
    // Scale and shift second cluster
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
auto particle_distribution::generate_random_vec() -> vec
{
    auto x = random_(random_engine_);
    auto y = random_(random_engine_);
    return vec(x, y);
}