#pragma once
#include "particle.h"
#include <vector>
#include <random>

class particle_distribution
{
public:
    enum class position_distribution
    {
        random_sphere,
        random_square,
        galaxy
    };

    enum class velocity_distribution
    {
        random,
        rotating,
        galaxy
    };

    particle_distribution();

    std::vector<particle> create_distribution(position_distribution pos_dist, 
                                              velocity_distribution vel_dist, 
                                              uint64_t n_particles,
                                              double max_speed, 
                                              bool add_central_body = true,
                                              double randomization_ratio = 0.0);
private:
    std::random_device random_device_;
    std::mt19937 random_engine_;
    std::uniform_real_distribution<double> random_;

    vec generate_random_vec();
};