#pragma once

#include <cstdint>

struct ControlChange
{
    int tick = 0;
    uint8_t channel = 0;
    uint8_t controller = 0;
    uint8_t value = 0;
};
