#include "tree.h"

#include <iostream>

// TODO: move to a more reasonable place
constexpr float_type theta = 0.5;
constexpr float_type epsilon = 10000;
constexpr float_type g_const = 0.1;

///
///
tree_node::tree_node(square_area area, tree_node* parent) : area_{area}, parent_{parent}, n_particles_{0}, level_{0}, mass_{0}, center_of_mass_{vec()}
{
    children_[0] = nullptr;
    children_[1] = nullptr;
    children_[2] = nullptr;
    children_[3] = nullptr;
}

///
///
auto tree_node::children() const -> const tree_node_array_type&
{
    return children_;
}

///
///
auto tree_node::area() const -> const square_area&
{
    return area_;
}

///
///
auto tree_node::insert_particle(particle* part) -> void
{
    if (part->pos().x() < area_.top_left_corner.x() 
        || part->pos().x() > area_.top_left_corner.x() + area_.side
        || part->pos().y() < area_.top_left_corner.y()
        || part->pos().y() > area_.top_left_corner.y() + area_.side)
    {
        //std::cout << "ERROR particle outside of quadrant\n" << *part;
        return;
    }

    if (n_particles_ == 0)
    {
        particle_ = part;
        ++n_particles_;
        return;
    }

    if (n_particles_ == 1)
    {
        if (!is_root() && !is_leaf())
        {
            std::cout << "ERROR node has particle but isn't a leaf\n";
            return;
        }

        // subdivide and relocate current particle
        auto quad = get_quadrant(particle_->pos());
        if (children_[static_cast<size_t>(quad)] == nullptr)
        {
            children_[static_cast<size_t>(quad)] = create_node_for_quadrant(quad);
        }
        children_[static_cast<int>(quad)]->insert_particle(particle_);

        particle_ = nullptr;
    }

    // insert new particle into its quadrant
    auto quad = get_quadrant(part->pos());
    if (children_[static_cast<size_t>(quad)] == nullptr)
    {
        children_[static_cast<size_t>(quad)] = create_node_for_quadrant(quad);
    }
    children_[static_cast<size_t>(quad)]->insert_particle(part);

    ++n_particles_;
}


///
///
auto tree_node::print_node() const -> void
{
    auto indent = std::string("");
    for (int i = 0; i < level_; ++i)
    {
        indent += "  ";
    }

    std::cout << indent << "n_particles: " << n_particles_ << ", particle: ";
    if (particle_ == nullptr)
    {
        std::cout << "none, ";
    }
    else
    {
        std::cout << particle_->pos() << ", ";
    } 
    std::cout << "top left corner: " << area_.top_left_corner << ", side length: " << area_.side << "\n";
    std::cout << indent << "center_of_mass: " << center_of_mass_ << ", mass: " << mass_ << "\n";

    for (const auto& child : children_)
    {
        if (child == nullptr)
        {
            continue;
        }
        child->print_node();
    }
}

///
///
auto tree_node::calculate_acceleration(const particle& part) -> vec
{
    // If it's the same particle as the one in the current node, no force
    if (particle_ == &part)
    {
        return vec(0.0, 0.0);
    }

    // If the distance is below a threshold, or already a leaf node, return force ...
    float_type inverse_dist = 1.0 / vec::distance(center_of_mass_, part.pos());
    if ((area_.side * inverse_dist) < theta || particle_ != nullptr)
    {
        auto denominator = inverse_dist * inverse_dist;
        if (denominator > epsilon)
        {
            denominator = epsilon;
        }
        return g_const * mass_ * denominator * (center_of_mass_ - part.pos()).normalized();
    }

    // ... otherwise, add up acceleration from child nodes
    auto force = vec(0.0, 0.0);
    for (const auto& child : children_)
    {
        if (child == nullptr)
        {
            continue;
        }
        force += child->calculate_acceleration(part);
    }

    return force;
}

///
///
auto tree_node::calculate_center_of_mass() -> void
{
    // if this is a leaf node with a single particle, its mass and position are used
    if (n_particles_ == 1)
    {
        center_of_mass_ = particle_->pos();
        mass_ = particle_->mass();
        return;
    }

    for (const auto& child : children_)
    {
        if (child == nullptr)
        {
            continue;
        }

        child->calculate_center_of_mass();
        mass_ += child->mass_;
        center_of_mass_ += child->mass_ * child->center_of_mass_;
    }
    center_of_mass_ /= mass_;
}

///
///
auto tree_node::get_quadrant(const vec& pos) -> quadrant
{
    auto is_left = pos.x() < area_.top_left_corner.x() + 0.5 * area_.side;
    auto is_top = pos.y() < area_.top_left_corner.y() + 0.5 * area_.side;

    if (is_left)
    {
        if (is_top)
        {
            return quadrant::top_left;
        }
        return quadrant::bottom_left;
    }
    if (is_top)
    {
        return quadrant::top_right;
    }
    return quadrant::bottom_right;
}

///
///
auto tree_node::create_node_for_quadrant(quadrant quad) const -> std::unique_ptr<tree_node>
{
    square_area new_area;
    float_type new_side = area_.side * 0.5;
    new_area.side = new_side;

    switch (quad)
    {
        case (quadrant::top_left):
        {
            new_area.top_left_corner = area_.top_left_corner;
            break;
        }
        case (quadrant::top_right):
        {
            new_area.top_left_corner.x() = area_.top_left_corner.x() + new_side;
            new_area.top_left_corner.y() = area_.top_left_corner.y();
            break;
        }
        case (quadrant::bottom_left):
        {
            new_area.top_left_corner.x() = area_.top_left_corner.x();
            new_area.top_left_corner.y() = area_.top_left_corner.y() + new_side;
            break;
        }
        case (quadrant::bottom_right):
        {
            new_area.top_left_corner.x() = area_.top_left_corner.x() + new_side;
            new_area.top_left_corner.y() = area_.top_left_corner.y() + new_side;
            break;
        }
        default:
        {
            std::cout << "ERROR creating node for invalid quadrant\n";
        }
    }

    auto new_node = std::make_unique<tree_node>(new_area, const_cast<tree_node*>(this));
    new_node->level_ = level_ + 1;
    return std::move(new_node);
}

///
///
auto tree_node::is_root() const -> bool
{
    return parent_ == nullptr;
}

///
///
auto tree_node::is_leaf() const -> bool
{
    return children_[0] == nullptr && children_[1] == nullptr && children_[2] == nullptr && children_[3] == nullptr;
}