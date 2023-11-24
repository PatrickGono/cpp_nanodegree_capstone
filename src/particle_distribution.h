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
    ///
    /// Creates the desired number of particles with the specified 
    /// position and velocity distribution.
    ///
    /// \param scenario Simulation scenario (i.e. two clusters, one cluster)
    /// \param pos_dist Position distribution choice
    /// \param vel_dist Velocity distribution choice
    /// \param n_particles Total number of particles to be created
    /// \param max_speed Maximum speed for particle velocities
    /// \param add_central_body If true, adds a central "black hole"
    /// \return Vector of created particles
    ///
    auto create_distribution(
        simulation_scenario scenario,
        position_distribution pos_dist, 
        velocity_distribution vel_dist, 
        uint64_t n_particles,
        float_type max_speed, 
        bool add_central_body = true) -> std::vector<particle>;

private: // Implementation
    /// 
    /// Emplaces particles with the desired position distribution
    /// into the provided container. See above for params.
    ///
    auto create_position_distribution(
        position_distribution pos_dist, 
        std::vector<particle>& particles,
        uint64_t n_particles,
        bool add_central_body) -> void;

    /// 
    /// Modifies particle velocities to achieve the desired position 
    /// distribution. See above for params.
    ///
    auto create_velocity_distribution(
        velocity_distribution vel_dist,
        std::vector<particle>& particles,
        float_type max_speed) -> void;

    /// 
    /// Convenience function that creates a cluster.
    ///
    /// \param center Location of the cluster center 
    /// \param velocity Average cluster velocity
    /// \param radius Cluster radius
    /// \param pos_dist Desired position distribution
    /// \param vel_dist Desired velocty distribution
    /// \param n_particles Number of particles in cluster
    /// \param max_speed Max speed for particle velocities
    /// \param add_central_body If true, adds a central "black hole"
    /// \return Container with cluster particles
    ///
    auto create_cluster(
        vec center,
        vec velocity,
        float_type radius,
        position_distribution pos_dist,
        velocity_distribution vel_dist,
        uint64_t n_particles,
        float_type max_speed,
        bool add_central_body) -> std::vector<particle>;

private: // Helpers
    ///
    /// Creates a vector with random x and y coordinates ranging 
    /// uniformly from 0.0 to 1.0. The vector is not normalized!
    /// \return Random vector
    ///
    auto generate_random_vec_uniform() -> vec;

    ///
    /// Creates a vector with random radial and angle coordinates following the 
    /// Cauchy distribution for the radial coordinate. The vector is not normalized!
    /// \return Random vector
    ///
    auto generate_random_vec_galaxy() -> vec;

private: // Variables
    std::random_device random_device_;
    std::mt19937 random_engine_;
    std::uniform_real_distribution<float_type> random_uniform_;
    std::cauchy_distribution<float_type> random_cauchy_;
};