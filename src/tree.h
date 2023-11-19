#pragma once
#include "particle.h"
#include "vec2.h"

#include <memory>

struct square_area
{
    square_area() : top_left_corner{vec()}, side(0.0) {}
    square_area(vec pos, float_type length) : top_left_corner{pos}, side(length) {}

    vec top_left_corner;
    float_type side;
};

enum class quadrant
{
    top_left,
    top_right,
    bottom_left,
    bottom_right,
};

class tree_node
{
public: // Structors
    tree_node(square_area area, tree_node* parent);

public: // Interface
    auto insert_particle(particle* part) -> void;
    auto print_node() const -> void;
    auto calculate_acceleration(const particle& part) -> vec;
    auto calculate_center_of_mass() -> void;

private: // Implementation 
    auto get_quadrant(const vec& pos) -> quadrant;
    auto create_node_for_quadrant(quadrant quad) const -> std::unique_ptr<tree_node>;
    auto is_root() const -> bool;
    auto is_leaf() const -> bool;

private: // Variables
    std::unique_ptr<tree_node> children_[4];
    tree_node* parent_ = nullptr;
    particle* particle_ = nullptr;
    size_t n_particles_ = 0;
    square_area area_; 
    float_type mass_;
    vec center_of_mass_;
    int level_;
};