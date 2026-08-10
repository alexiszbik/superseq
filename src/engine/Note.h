#pragma once

#include <cstdint>

struct Note
{
    int tick = 0; //todo : turn it to uint16_t
    int durationTicks = 0; //todo : turn it to uint16_t
    uint8_t note = 0;
    uint8_t velocity = 0;
};
