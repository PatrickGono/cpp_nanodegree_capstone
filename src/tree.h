#pragma once
#include "particle.h"
#include "vec2.h"

#include <memory>

// Forward declaration
class tree_node;

// Typedefs
typedef std::unique_ptr<tree_node> tree_node_array_type[4];

struct square_area
{
    square_area() : top_left_corner{vec()}, side(0.0) {}
    square_area(vec pos, float_type length) : top_left_corner{pos}, side(length) {}

    vec top_left_corner;
    float_type side;
};

///
/// A node in a quad tree. Has up to 4 children, and can hold a particle.
///
class tree_node
{
public: // Enums
    enum class quadrant
    {
        top_left,
        top_right,
        bottom_left,
        bottom_right,
    };

public: // Structors
    tree_node(square_area area, tree_node* parent);

public: // Accessors
    auto children() const -> const tree_node_array_type&;
    auto area() const -> const square_area&;

public: // Interface
    ///
    /// Inserts the particle into the tree. Makes sure that at most 
    /// 1 particle exists in any node by recursive subdivision. 
    /// Doesn't take ownership of the particle object.
    ///
    /// \param part Particle to be inserted
    ///
    auto insert_particle(particle* part) -> void;

    /// 
    /// Debug print-out of the node and all its children.
    ///
    auto print_node() const -> void;

    ///
    /// Calculates the acceleration felt by the particle due to tree.
    /// \param part Particle for which to calculate the acceleration
    /// \param theta Threshold parameter (0.0 - 1.0): smaller = more accurate
    /// \return Acceleration
    ///
    auto calculate_acceleration(const particle& part, float_type theta) const -> vec;

    ///
    /// Calculates the mass statistics for all children, recursively.
    /// 
    auto calculate_center_of_mass() -> void;

private: // Implementation 
    ///
    /// Returns into which quadrant of the node the position falls.
    /// \param pos Position within the node area
    /// \return Quadrant into which the position falls
    ///
    auto get_quadrant(const vec& pos) -> quadrant;

    /// 
    /// Creates a child node for the specified quadrant.
    /// \param quad Quadrant for which to add child node
    /// \return Unique pointer to the new node
    ///
    auto create_node_for_quadrant(quadrant quad) const -> std::unique_ptr<tree_node>;

    /// 
    /// \return True if this node is the root of the treea
    ///
    auto is_root() const -> bool;

    /// 
    /// \return True if this node is a leaf (contains particle, no children)
    ///
    auto is_leaf() const -> bool;

private: // Variables
    tree_node_array_type children_;
    tree_node* parent_ = nullptr;
    particle* particle_ = nullptr;
    size_t n_particles_ = 0;
    square_area area_; 
    float_type mass_;
    vec center_of_mass_;
    int level_;
};