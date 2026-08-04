#include "SequenceFactory.h"

#include "SequenceTrackFactory.h"

namespace
{
using TrackBuilder = SequenceTrack (*)(int lengthInTicks, int beatDuration);

Sequence buildSequence(
    int barCount,
    int beatsPerBar,
    int barLoop,
    const char* name,
    TrackBuilder first,
    TrackBuilder second)
{
    Sequence sequence(name, barCount, beatsPerBar, barLoop, Sequence::kTicksPerQuarterNote);
    const int length = sequence.lengthInTicks();
    const int beatDuration = sequence.beatDuration();

    sequence.addTrack(first(length, beatDuration));
    sequence.addTrack(second(length, beatDuration));

    return sequence;
}
} // namespace

Sequence SequenceFactory::createSequenceOne(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Kick/Hats + Pads",
        SequenceTrackFactory::createKickSnareWithHats,
        SequenceTrackFactory::createCMaj7Arpeggio);
}

Sequence SequenceFactory::createSequenceTwo(int barCount)
{
    return buildSequence(
        barCount, 4, 2, "Cmaj7 + Four on Floor",
        SequenceTrackFactory::createCMaj7Arpeggio,
        SequenceTrackFactory::createFourOnFloorKick);
}

Sequence SequenceFactory::createSequenceThree(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Am7 + Hi-Hat",
        SequenceTrackFactory::createAm7Arpeggio,
        SequenceTrackFactory::createHiHatPattern);
}

Sequence SequenceFactory::createSequenceFour(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Pads + Kick/Snare",
        SequenceTrackFactory::createPadChords,
        SequenceTrackFactory::createKickSnare);
}

Sequence SequenceFactory::createSequenceFive(int barCount)
{
    return buildSequence(
        barCount, 4, 1, "Melodic Bass + Snare",
        SequenceTrackFactory::createMelodicBass,
        SequenceTrackFactory::createSnareBackbeat);
}

Sequence SequenceFactory::createSequenceSix(int barCount)
{
    return buildSequence(
        barCount, 4, 2, "Synth Stabs + Kick/Hats",
        SequenceTrackFactory::createSynthStabs,
        SequenceTrackFactory::createKickSnareWithHats);
}

Sequence SequenceFactory::createSequenceSeven(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Clap + Melodic Bass",
        SequenceTrackFactory::createClapBackbeat,
        SequenceTrackFactory::createMelodicBass);
}
