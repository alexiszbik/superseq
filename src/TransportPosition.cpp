#include "TransportPosition.h"

TransportPosition TransportPosition::fromTickIndex(
    tick_t tickIndex,
    int beatsPerBar,
    tick_t lengthInTicks)
{
    const int ticksPerBar = beatsPerBar * kTicksPerQuarterNote;
    const tick_t clampedTick = tickIndex >= lengthInTicks ? lengthInTicks - 1 : tickIndex;

    TransportPosition position;
    position.bar = clampedTick / ticksPerBar + 1;

    const int tickInBar = clampedTick % ticksPerBar;
    position.beat = tickInBar / kTicksPerQuarterNote + 1;
    position.tick = tickInBar % kTicksPerQuarterNote;

    return position;
}

std::string TransportPosition::toString() const
{
    return std::to_string(bar) + "." + std::to_string(beat) + "." + std::to_string(tick);
}
