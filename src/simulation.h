#pragma once

#include "camera.h"
#include "particle.h"
#include "particle_distribution.h"
#include "renderer.h"

class simulation
{
public: // Structors
    simulation(uint64_t n_particles); 

public: // Interface
    auto run(renderer &renderer) -> void;
  
private: // Implementation
    auto update() -> void;
    auto update_thread() -> void;
    auto update_barnes_hut() -> void;
    auto compute_total_energy() -> double;

private: // Variables
    uint64_t n_particles_;
    std::vector<particle> particles_;
    particle_distribution initial_distribution_;
    camera camera_;
};