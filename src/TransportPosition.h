#pragma once

#include "Tick.h"

#include <string>

class TransportPosition
{
public:
    static constexpr int kTicksPerQuarterNote = 96;

    int bar = 1;
    int beat = 1;
    int tick = 0;

    static TransportPosition fromTickIndex(
        tick_t tickIndex,
        int beatsPerBar,
        tick_t lengthInTicks);

    std::string toString() const;
};
