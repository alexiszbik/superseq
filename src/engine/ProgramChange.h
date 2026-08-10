#pragma once

#include <cstdint>

struct ProgramChange
{
    int tick = 0; //todo : turn it to uint16_t
    uint8_t program = 0;
};
