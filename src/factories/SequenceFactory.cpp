#include "SequenceFactory.h"

#include "SequenceTrackFactory.h"

namespace
{
using TrackBuilder = SequenceTrack (*)(int lengthInTicks, int beatDuration);

Sequence buildSequence(int barCount, int beatsPerBar, int barLoop, TrackBuilder trackBuilder)
{
    Sequence sequence(barCount, beatsPerBar, barLoop);
    const int length = sequence.lengthInTicks();
    const int beatDuration = sequence.beatDuration();

    sequence.addTrack(trackBuilder(length, beatDuration));

    return sequence;
}

Sequence buildSequence(int barCount, int beatsPerBar, int barLoop, TrackBuilder first, TrackBuilder second)
{
    Sequence sequence(barCount, beatsPerBar, barLoop);
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
        barCount, 4, 2,
        SequenceTrackFactory::createCMaj7Arpeggio,
        SequenceTrackFactory::createKickSnare);
}

Sequence SequenceFactory::createSequenceTwo(int barCount)
{
    return buildSequence(
        barCount, 4, 0,
        SequenceTrackFactory::createKickSnareWithHats,
        SequenceTrackFactory::createBassLine);
}

Sequence SequenceFactory::createSequenceThree(int barCount)
{
    return buildSequence(
        barCount, 4, 0,
        SequenceTrackFactory::createAm7Arpeggio,
        SequenceTrackFactory::createHiHatPattern);
}

Sequence SequenceFactory::createSequenceFour(int barCount)
{
    return buildSequence(
        barCount, 4, 0,
        SequenceTrackFactory::createPadChords,
        SequenceTrackFactory::createFourOnFloorKick);
}

Sequence SequenceFactory::createSequenceFive(int barCount)
{
    return buildSequence(
        barCount, 4, 1,
        SequenceTrackFactory::createMelodicBass,
        SequenceTrackFactory::createSnareBackbeat);
}

Sequence SequenceFactory::createSequenceSix(int barCount)
{
    return buildSequence(
        barCount, 4, 2,
        SequenceTrackFactory::createSynthStabs,
        SequenceTrackFactory::createKickSnareWithHats);
}

Sequence SequenceFactory::createSequenceSeven(int barCount)
{
    return buildSequence(
        barCount, 4, 0,
        SequenceTrackFactory::createClapBackbeat,
        SequenceTrackFactory::createMelodicBass);
}
