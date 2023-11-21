#pragma once
#include "vec2.h"
#include "particle.h"

///
/// Class representing a camera, with a position and zoom level/size.
/// Used to transform world space coordinates to window coordinates.
///
class camera
{
public: // Structors
    camera();

public: // Interface
    ///
    /// Transforms the position from world coordinates to window 
    /// coordinates (0.0 to 1.0) for the current camera position.
    /// \param position Position in world coords to be transformed
    ///
    auto transform(const vec& position) const -> vec;

    ///
    /// Transforms a distance/length from world coordinates to 
    /// window coordinates for the current camera zoom.
    /// \param length Length in world coords to be transformed
    ///
    auto transform(float_type length) const -> float_type;

    ///
    /// Translate camera one step in the desired direction along x.
    /// \param left If true, translate left, if false, right
    /// 
    auto translate_x(bool left) -> void;

    ///
    /// Translate camera one step in the desired direction along y.
    /// \param up If true, translate up, if false, down 
    /// 
    auto translate_y(bool up) -> void;

    ///
    /// Zoom camera one step either in or out.
    /// \param in If true, zoom in, if false, zoom out
    /// 
    auto zoom(bool in) -> void;

private:
    float_type zoom_level_;
    vec position_;
};