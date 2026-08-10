#pragma once

#include "Tick.h"

#include <cstdint>

struct Note
{
    tick_t tick = 0;
    tick_t durationTicks = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
};
