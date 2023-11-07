#pragma once
#include "vec2.h"

#include <iostream>

using float_type = double;
using vec = vec2<float_type>;

class particle
{
public:
    particle(vec pos) : position_{pos}, velocity_{}, mass_{1} {}
    particle(vec pos, float_type mass) : position_{pos}, velocity_{}, mass_{mass} {}
    particle(vec pos, vec vel) : position_{pos}, velocity_{vel}, mass_{1} {}
    particle(vec pos, vec vel, float_type mass) : position_{pos}, velocity_{vel}, mass_{mass} {}

    vec& pos() { return position_; }
    const vec& pos() const { return position_; }
    vec& vel() { return velocity_; }
    float_type mass() const { return mass_; }

    friend std::ostream& operator<<(std::ostream& cout, const particle& part)
    {
        return cout << "mass = " << part.mass_ << "; pos = " << part.position_ << "; vel = " << part.velocity_ << "\n";
    }

private:
    vec position_;
    vec velocity_;
    float_type mass_;
};