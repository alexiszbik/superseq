#include "SequenceFactory.h"

#include "SequenceTrackFactory.h"

namespace
{
using TrackBuilder = SequenceTrack (*)(tick_t lengthInTicks);

Sequence buildSequence(
    int barCount,
    int beatsPerBar,
    int barLoop,
    const char* name,
    std::vector<TrackBuilder> builders)
{
    Sequence sequence(name, barCount, beatsPerBar, barLoop);
    const tick_t length = sequence.lengthInTicks();

    for (auto& tb : builders) {
        sequence.addTrack(tb(length));
    }
    return sequence;
}
} // namespace

Sequence SequenceFactory::createSequenceOne(int barCount)
{
    return buildSequence(
        2, 7, 1, "Kick/Hats + Pads",
        {
            SequenceTrackFactory::createKickSnareWithHats, 
            SequenceTrackFactory::createCMaj7Arpeggio
        });
}

Sequence SequenceFactory::createSequenceTwo(int barCount)
{
    return buildSequence(
        barCount, 4, 2, "Cmaj7 + Four on Floor", 
        {
            SequenceTrackFactory::createCMaj7Arpeggio,
            SequenceTrackFactory::createFourOnFloorKick
        });
}

Sequence SequenceFactory::createSequenceThree(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Am7 + Hi-Hat",
        {
            SequenceTrackFactory::createAm7Arpeggio,
            SequenceTrackFactory::createHiHatPattern
        });
}

Sequence SequenceFactory::createSequenceFour(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Pads + Kick/Snare",
        {
            SequenceTrackFactory::createPadChords,
            SequenceTrackFactory::createKickSnare
        });
}

Sequence SequenceFactory::createSequenceFive(int barCount)
{
    return buildSequence(
        barCount, 4, 1, "Melodic Bass + Snare",
        {
            SequenceTrackFactory::createMelodicBass,
            SequenceTrackFactory::createSnareBackbeat
        });
}

Sequence SequenceFactory::createSequenceSix(int barCount)
{
    return buildSequence(
        barCount, 4, 2, "Synth Stabs + Kick/Hats",
        {
            SequenceTrackFactory::createSynthStabs,
            SequenceTrackFactory::createKickSnareWithHats
        });
}

Sequence SequenceFactory::createSequenceSeven(int barCount)
{
    return buildSequence(
        barCount, 4, 0, "Clap + Melodic Bass",
        {
            SequenceTrackFactory::createClapBackbeat,
            SequenceTrackFactory::createMelodicBass
        });
}
