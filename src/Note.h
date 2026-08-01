#pragma once

#include <cstdint>

struct Note
{
    int tick = 0;
    int durationTicks = 0;
    uint8_t channel = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
};
