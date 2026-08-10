#pragma once

#include "SequenceTrack.h"

class SequenceTrackFactory
{
public:
    static SequenceTrack createCMaj7Arpeggio(tick_t lengthInTicks);
    static SequenceTrack createAm7Arpeggio(tick_t lengthInTicks);
    static SequenceTrack createKickSnare(tick_t lengthInTicks);
    static SequenceTrack createKickSnareWithHats(tick_t lengthInTicks);
    static SequenceTrack createBassLine(tick_t lengthInTicks);
    static SequenceTrack createMelodicBass(tick_t lengthInTicks);
    static SequenceTrack createHiHatPattern(tick_t lengthInTicks);
    static SequenceTrack createFourOnFloorKick(tick_t lengthInTicks);
    static SequenceTrack createSnareBackbeat(tick_t lengthInTicks);
    static SequenceTrack createPadChords(tick_t lengthInTicks);
    static SequenceTrack createSynthStabs(tick_t lengthInTicks);
    static SequenceTrack createClapBackbeat(tick_t lengthInTicks);
};
