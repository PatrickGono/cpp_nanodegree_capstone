#pragma once
#include "vec2.h"
#include "particle.h"

class camera
{
public: // Structors
    camera();

public: // Interface
    auto transform(const vec& position) const -> vec;
    auto translate_x(bool left) -> void;
    auto translate_y(bool up) -> void;
    auto zoom(bool in) -> void;

private:
    double zoom_level_;
    vec position_;
};