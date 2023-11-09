#pragma once

#include <random>
#include "SDL.h"
#include "renderer.h"
#include "particle.h"

class simulation
{
public:
    simulation(uint64_t n_particles); 
    void run(renderer &renderer);
  
private:
    std::random_device random_device_;
    std::mt19937 random_engine_;
    std::uniform_real_distribution<double> random_;
    uint64_t n_particles_;
    std::vector<particle> particles_;
  
    void update();
    double compute_total_energy();
};