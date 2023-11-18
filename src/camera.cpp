#include "camera.h"

#include "vec2.h"

constexpr float_type camera_translation_speed = 0.025;
constexpr float_type camera_zoom_speed = 0.05;

///
///
camera::camera() : position_(vec(0)), zoom_level_(1.0)
{
}

///
///
auto camera::transform(const vec& position) const -> vec
{
    const auto center = position_ - vec(0.5 * zoom_level_);
    auto dist = position - center;
    return dist / zoom_level_;
}

///
///
auto camera::translate_x(bool left) -> void
{
    position_.x() += zoom_level_ * (left ? -camera_zoom_speed : camera_zoom_speed);
}

///
///
auto camera::translate_y(bool left) -> void
{
    position_.y() += zoom_level_ * (left ? -camera_zoom_speed : camera_zoom_speed);
}

///
///
auto camera::zoom(bool in) -> void
{
    zoom_level_ *= in ? (1 - camera_zoom_speed) : (1 + camera_zoom_speed);
}

