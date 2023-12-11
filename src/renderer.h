#pragma once
#include "camera.h"
#include "particle.h"
#include "tree.h"

#include "SDL.h"

#include <vector>

struct color
{
    constexpr color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
    color() : r(0), g(0), b(0), a(0) {}
    int r, g, b, a;
};

///
/// Renderer class responsible for creating an SDL window and rendering onto it.
///
class renderer 
{
public: // Structs
  struct sdl_deleter 
  {
    void operator()(SDL_Renderer* ptr) { if (ptr != nullptr) { SDL_DestroyRenderer(ptr); }}
    void operator()(SDL_Window* ptr) { if (ptr != nullptr) { SDL_DestroyWindow(ptr); }}
  };
  
  using renderer_ptr = std::unique_ptr<SDL_Renderer, sdl_deleter>;
  using window_ptr = std::unique_ptr<SDL_Window, sdl_deleter>;

public: // Structors
    renderer(int screen_width, int screen_height);
    ~renderer();

public: // Interface
    ///
    /// Renders particles as a group of points using the camera view.
    /// \param particles Container of particles to render
    /// \param cam Handle to camera object
    ///
    auto render(const std::vector<particle>& particles, const camera& cam) -> void;

    ///
    /// Renders particles as a group of points using the camera view.
    /// Also renders the quad tree as a collection of rectangles.
    ///
    /// \param tree Quad tree to render
    /// \param particles Container of particles to render
    /// \param cam Handle to camera object
    ///
    auto render(const tree_node& tree, const std::vector<particle>& particles, const camera& cam) -> void;

    /// 
    /// Updates the title of the window with the following information:
    /// \param n_particles Number of particles
    /// \param fps Frames per second 
    /// 
    auto update_window_title(uint64_t n_particles, int fps) -> void;

private: // Implementation
    ///
    /// Helper function doing the actual particle rendering.
    /// \param particles Container of particles to render
    /// \param cam Handle to camera object
    ///
    auto render_particles(const std::vector<particle>& particles, const camera& cam) -> void;

    ///
    /// Helper function doing the actual tree rendering.
    /// \param tree Quad tree to render
    /// \param cam Handle to camera object
    ///
    auto render_tree(const tree_node& tree, const camera& cam) -> void;

    ///
    /// Adds a rectangle for each node, recursively traversing the tree.
    /// \param rectangles Container into which to add the rectangles
    /// \param tree Quad tree to render
    /// \param cam Handle to camera object
    ///
    auto render_tree_nodes(std::vector<SDL_Rect>& rectangles, const tree_node& tree, const camera& cam) -> void;

private: // Variables
    const std::size_t screen_width_;
    const std::size_t screen_height_;
    window_ptr sdl_window_;
    renderer_ptr sdl_renderer_;
};