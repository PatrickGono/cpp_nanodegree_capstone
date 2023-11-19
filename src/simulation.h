#pragma once

#include "camera.h"
#include "particle.h"
#include "particle_distribution.h"
#include "renderer.h"

class simulation
{
public: // Enums
    enum class algorithm
    {
        brute_force,
        parallel_brute_force,
        barnes_hut
    };

public: // Structors
    simulation(uint64_t n_particles); 

public: // Accessors
    auto get_camera() -> camera&;
    auto get_running() -> bool&;
    auto get_algorithm() -> algorithm&;

public: // Interface
    auto run(renderer &renderer) -> void;
  
private: // Implementation
    auto update() -> void;

    auto calculate_brute_force(std::vector<vec>& accelerations) const -> void;

    auto calculate_brute_force_parallel(std::vector<vec>& accelerations) const -> void;

    auto calculate_barnes_hut(std::vector<vec>& accelerations) -> void;

    auto compute_total_energy() -> float_type;

private: // Variables
    uint64_t n_particles_;
    std::vector<particle> particles_;
    particle_distribution initial_distribution_;
    algorithm algorithm_;
    camera camera_;
    bool running_;
};