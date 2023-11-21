#pragma once
#include "simulation.h"

#include "SDL.h"

///
/// Simple keyboard and mouse event handler.
///
class controller 
{
public: // Interface
    ///
    /// Handle all input (keybaord, mouse, and window closing).
    /// \param sim Handle to simulation object
    ///
    static auto handle_input(simulation& sim) -> void;

private: // Implementation
    ///
    /// Handle keybaord input.
    /// \param event SDL event
    /// \param sim Handle to simulation object
    ///
    static auto handle_keyboard(const SDL_Event& event, simulation& sim) -> void;

    ///
    /// Handle mouse input.
    /// \param event SDL event
    /// \param sim Handle to simulation object
    ///
    static auto handle_mouse(const SDL_Event& event,  simulation& sim) -> void;
};