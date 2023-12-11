#pragma once

#include "camera.h"
#include "particle.h"
#include "particle_distribution.h"
#include "renderer.h"
#include "tree.h"

#include <future>
#include <mutex>
#include <vector>

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
    /// 
    /// Constructor
    /// \param n_particles Initial number of particles
    ///
    simulation(uint64_t n_particles); 

public: // Accessors
    ///
    /// \return Access to camera object
    ///
    auto get_camera() -> camera&;

    ///
    /// \return Access to simulation state object
    ///
    auto get_state() -> state&;

    ///
    /// \return Access to quad tree render flag
    ///
    auto get_render_quad_tree() -> bool&;

    /// 
    /// \return Access to algorithm enum (which algorithm to be used for accelerations) 
    /// 
    auto get_algorithm() -> algorithm&;

public: // Modifiers
    ///
    /// Slows down the simulation by reducing the time step delta_t.
    ///
    auto slow_down_simulation() -> void;

    ///
    /// Reverses the simulation by flipping the sign on the time step delta_t.
    ///
    auto reverse_simulation() -> void;

    ///
    /// Speeds up the simulation by increasing the time step delta_t.
    ///
    auto speed_up_simulation() -> void;

    ///
    /// Sets the simulation scenario and restarts the simulation.
    /// Remaining settings are kept unchanged.
    /// \param scenario Scenario enum to be selected
    ///
    auto set_scenario(particle_distribution::simulation_scenario scenario) -> void;

    ///
    /// Increases the particle count by 1000 and restarts the simulation.
    /// Remaining settings are kept unchanged.
    ///
    auto increase_particles_by_1000_and_restart() -> void;

    ///
    /// Decreases the particle count by 1000 and restarts the simulation.
    /// Remaining settings are kept unchanged.
    ///
    auto decrease_particles_by_1000_and_restart() -> void;

    ///
    /// Increases the theta parameter used in the Barnes-Hut algorithm.
    /// This reduces accuracy but makes the calculations faster.
    ///
    auto increase_theta() -> void;

    ///
    /// Decreases the theta parameter used in the Barnes-Hut algorithm.
    /// This increases accuracy but makes the calculations slower.
    ///
    auto decrease_theta() -> void;

public: // Interface
    ///
    /// Core method of the class. Starts the simulation and the update loop.
    /// \param Access to renderer used for rendering at each update step.
    ///
    auto run(renderer& renderer) -> void;
  
private: // Implementation
    ///
    /// Resets the simulation and initializes relevant parameters.
    ///
    auto init() -> void;

    ///
    /// Starts the simulation update thread using std::async
    /// \param Access to renderer used for rendering at each update step.
    ///
    auto start_simulation_thread(renderer& renderer) -> void;

    ///
    /// Updates the simulation using the velocity Verlet algorithm.
    ///
    auto update() -> void;

    ///
    /// Uses the brute force algorithm to compute accelerations.
    /// \param accelerations Container of accelerations to be filled on return
    ///
    auto calculate_brute_force(std::vector<vec>& accelerations) const -> void;

    ///
    /// Uses the brute force algorithm and std::thread to compute accelerations.
    /// \param accelerations Container of accelerations to be filled on return
    ///
    auto calculate_brute_force_threads(std::vector<vec>& accelerations) const -> void;
    
    ///
    /// Uses the brute force algorithm and std::async to compute accelerations.
    /// \param accelerations Container of accelerations to be filled on return
    ///
    auto calculate_brute_force_async(std::vector<vec>& accelerations) const -> void;

    ///
    /// Uses the Barnes-Hut algorithm to compute accelerations.
    /// \param accelerations Container of accelerations to be filled on return
    ///
    auto calculate_barnes_hut(std::vector<vec>& accelerations) -> void;

    ///
    /// Uses the Barnes-Hut algorithm and std::thread to compute accelerations.
    /// \param accelerations Container of accelerations to be filled on return
    ///
    auto calculate_barnes_hut_threads(std::vector<vec>& accelerations) -> void;

    ///
    /// Creates a quad tree from the particles.
    /// \param update_area If true, updates the area after every couple of frames
    /// \return The root node of the created tree
    ///
    auto create_quad_tree(bool update_area = false) -> tree_node;

    ///
    /// Calculates the square enclosing all particles.
    /// \return Square bounds
    ///
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
    std::mutex controller_mutex_;
    std::future<void> simulation_thread_future_;
};