#pragma once
#include "vec2.h"

#include <iostream>

using float_type = double;
using vec = vec2<float_type>;

class particle
{
public: // Structors
    particle(vec pos) : position_{pos}, velocity_{}, mass_{1} {}
    particle(vec pos, float_type mass) : position_{pos}, velocity_{}, mass_{mass} {}
    particle(vec pos, vec vel) : position_{pos}, velocity_{vel}, mass_{1} {}
    particle(vec pos, vec vel, float_type mass) : position_{pos}, velocity_{vel}, mass_{mass} {}

public: // Accessors
    auto pos() -> vec& { return position_; }
    auto vel() -> vec& { return velocity_; }
    auto acc() -> vec& { return acceleration_; }
    auto pos() const -> const vec& { return position_; }
    auto vel() const -> const vec& { return velocity_; }
    auto acc() const -> const vec& { return acceleration_; }
    float_type mass() const { return mass_; }

public: // Operators
    friend auto operator<<(std::ostream& cout, const particle& part) -> std::ostream&
    {
        return cout << "mass = " << part.mass_ << "; pos = " << part.position_ << "; vel = " << part.velocity_  << "; acc = " << part.acceleration_ << "\n";
    }

private: // Variables
    vec position_;
    vec velocity_;
    vec acceleration_;
    float_type mass_;
};