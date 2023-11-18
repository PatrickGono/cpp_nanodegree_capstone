#pragma once
#include "camera.h"

class controller 
{
public: // Interface
    static auto handle_input(bool& running, camera& cam) -> void;
};