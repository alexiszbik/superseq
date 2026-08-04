#pragma once

#include "SequenceTrack.h"

class SequenceTrackFactory
{
public:
    static SequenceTrack createCMaj7Arpeggio(int lengthInTicks);
    static SequenceTrack createAm7Arpeggio(int lengthInTicks);
    static SequenceTrack createKickSnare(int lengthInTicks);
    static SequenceTrack createKickSnareWithHats(int lengthInTicks);
    static SequenceTrack createBassLine(int lengthInTicks);
    static SequenceTrack createMelodicBass(int lengthInTicks);
    static SequenceTrack createHiHatPattern(int lengthInTicks);
    static SequenceTrack createFourOnFloorKick(int lengthInTicks);
    static SequenceTrack createSnareBackbeat(int lengthInTicks);
    static SequenceTrack createPadChords(int lengthInTicks);
    static SequenceTrack createSynthStabs(int lengthInTicks);
    static SequenceTrack createClapBackbeat(int lengthInTicks);
};
