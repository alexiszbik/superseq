#pragma once

#include <string>

class TransportPosition
{
public:
    int bar = 1;
    int beat = 1;
    int tick = 0;

    static TransportPosition fromTickIndex(
        int tickIndex,
        int beatsPerBar,
        int beatDuration,
        int lengthInTicks);

    std::string toString() const;
};
