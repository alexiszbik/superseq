#include "TransportPosition.h"

#include "Sequence.h"

TransportPosition TransportPosition::fromTickIndex(
    int tickIndex,
    int beatsPerBar,
    int lengthInTicks)
{
    const int beatDuration = Sequence::kTicksPerQuarterNote;
    const int ticksPerBar = beatsPerBar * beatDuration;
    const int clampedTick = tickIndex >= lengthInTicks ? lengthInTicks - 1 : tickIndex;

    TransportPosition position;
    position.bar = clampedTick / ticksPerBar + 1;

    const int tickInBar = clampedTick % ticksPerBar;
    position.beat = tickInBar / beatDuration + 1;
    position.tick = tickInBar % beatDuration;

    return position;
}

std::string TransportPosition::toString() const
{
    return std::to_string(bar) + "." + std::to_string(beat) + "." + std::to_string(tick);
}
