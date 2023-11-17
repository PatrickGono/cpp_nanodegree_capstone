#pragma once

class controller 
{
public: // Interface
    static auto handle_input(bool &running) -> void;
};