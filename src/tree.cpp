#include "tree.h"
#include <iostream>

constexpr float_type theta = 0.1;

//
//
tree_node::tree_node(square_area area, tree_node* parent) : area_{area}, parent_{parent}, n_particles_{0}, level_{0}
{
    children_[0] = nullptr;
    children_[1] = nullptr;
    children_[2] = nullptr;
    children_[3] = nullptr;
}

//
//
auto tree_node::is_root() const -> bool
{
    return parent_ == nullptr;
}

//
//
auto tree_node::is_leaf() const -> bool
{
    children_[0] == nullptr && children_[1] == nullptr && children_[2] == nullptr && children_[3] == nullptr;
}

//
//
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
    std::cout << "quadrant: " << area_.top_left_corner << ", side length: " << area_.side << "\n";

    for (const auto& child : children_)
    {
        if (child == nullptr)
        {
            continue;
        }
        child->print_node();
    }
}

//
//
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

//
//
auto tree_node::insert_particle(particle* part) -> void
{
    if (level_ > 10)
    {
        std::cout << "ERROR reached max level\n";
        return;
    }
    if (part->pos().x() < area_.top_left_corner.x() 
        || part->pos().x() > area_.top_left_corner.x() + area_.side
        || part->pos().y() < area_.top_left_corner.y()
        || part->pos().y() > area_.top_left_corner.y() + area_.side)
    {
        std::cout << "ERROR particle outside of quadrant\n";
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

//
//
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