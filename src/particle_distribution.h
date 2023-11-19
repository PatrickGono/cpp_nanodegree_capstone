#pragma once
#include "particle.h"
#include <vector>
#include <random>

class particle_distribution
{
public: // Enums
    enum class simulation_scenario
    {
        one_cluster,
        two_clusters,
        cluster_and_black_hole
    };

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

public: // Structors
    particle_distribution();

public: // Interface
    auto create_distribution(
        simulation_scenario scenario,
        position_distribution pos_dist, 
        velocity_distribution vel_dist, 
        uint64_t n_particles,
        float_type max_speed, 
        bool add_central_body = true,
        double randomization_ratio = 0.0) -> std::vector<particle>;

private: // Implementation
    auto create_position_distribution(
        position_distribution pos_dist, 
        std::vector<particle>& particles,
        uint64_t n_particles,
        bool add_central_body) -> void;

    auto create_velocity_distribution(
        velocity_distribution vel_dist,
        std::vector<particle>& particles,
        float_type max_speed,
        float_type randomization_ratio) -> void;

private: // Helpers
    auto generate_random_vec() -> vec;

private: // Variables
    std::random_device random_device_;
    std::mt19937 random_engine_;
    std::uniform_real_distribution<double> random_;
};