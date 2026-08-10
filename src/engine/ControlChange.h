#pragma once

#include <cstdint>

struct ControlChange
{
    int tick = 0; //todo : turn it to uint16_t
    uint8_t controller = 0;
    uint8_t value = 0;
};
