#pragma once

#include "Tick.h"

#include <cstdint>

struct ProgramChange
{
    tick_t tick = 0;
    uint8_t program = 0;
};
