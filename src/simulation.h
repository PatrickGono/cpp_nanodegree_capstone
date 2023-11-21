#pragma once

#include "camera.h"
#include "particle.h"
#include "particle_distribution.h"
#include "renderer.h"
#include "tree.h"

class simulation
{
public: // Enums
    enum class algorithm
    {
        brute_force,
        brute_force_threads,
        brute_force_async,
        barnes_hut,
        barnes_hut_threads
    };

public: // Structors
    simulation(uint64_t n_particles); 

public: // Accessors
    auto get_camera() -> camera&;
    auto get_running() -> bool&;
    auto get_render_quad_tree() -> bool&;
    auto get_algorithm() -> algorithm&;

public: // Interface
    auto run(renderer &renderer) -> void;
  
private: // Implementation
    auto update() -> void;

    auto calculate_brute_force(std::vector<vec>& accelerations) const -> void;

    auto calculate_brute_force_threads(std::vector<vec>& accelerations) const -> void;
    
    auto calculate_brute_force_async(std::vector<vec>& accelerations) const -> void;

    auto calculate_barnes_hut(std::vector<vec>& accelerations) -> void;

    auto calculate_barnes_hut_threads(std::vector<vec>& accelerations) -> void;

    auto create_quad_tree() -> tree_node;

    auto calculate_particles_bounds() const -> square_area;

private: // Variables
    uint64_t n_particles_;
    std::vector<particle> particles_;
    particle_distribution initial_distribution_;
    algorithm algorithm_;
    camera camera_;
    bool running_;
    bool render_quad_tree_;
    int frame_count_;
    square_area area_;
};