#pragma once

#include "Tick.h"

#include <cstdint>

struct MuteEvent
{
    tick_t tick = 0;
    //now we assume that mute event is always to mute back
};
