#pragma once

#include <cstdint>

struct ProgramChange
{
    int tick = 0;
    uint8_t channel = 0;
    uint8_t program = 0;
};
