#pragma once

#include <random>
#include "SDL.h"
#include "renderer.h"
#include "particle_distribution.h"
#include "particle.h"

class simulation
{
public:
    simulation(uint64_t n_particles); 
    void run(renderer &renderer);
  
private:
    uint64_t n_particles_;
    std::vector<particle> particles_;
    particle_distribution initial_distribution_;
  
    void update();
    double compute_total_energy();
};