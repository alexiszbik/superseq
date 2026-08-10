#pragma once

#include "Tick.h"

#include <cstdint>

struct ControlChange
{
    tick_t tick = 0;
    uint8_t controller = 0;
    uint8_t value = 0;
};
