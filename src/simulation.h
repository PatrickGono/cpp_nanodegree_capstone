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

    enum class state
    {
        running,
        paused,
        exiting
    };

public: // Structors
    simulation(uint64_t n_particles); 

public: // Accessors
    auto get_camera() -> camera&;
    auto get_state() -> state&;
    auto get_render_quad_tree() -> bool&;
    auto get_algorithm() -> algorithm&;

public: // Modifiers
    auto slow_down_simulation() -> void;
    auto reverse_simulation() -> void;
    auto speed_up_simulation() -> void;
    auto set_scenario(particle_distribution::simulation_scenario scenario) -> void;
    auto increase_particles_by_1000_and_restart() -> void;
    auto decrease_particles_by_1000_and_restart() -> void;
    auto increase_theta() -> void;
    auto decrease_theta() -> void;

public: // Interface
    auto run(renderer &renderer) -> void;
  
private: // Implementation
    auto init() -> void;

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
    particle_distribution::simulation_scenario scenario_;
    algorithm algorithm_;
    bool render_quad_tree_;
    int frame_count_;
    state state_;
    camera camera_;
    square_area area_;
    float_type delta_t_;
    float_type half_delta_t_squared_;
    float_type theta_;
};