#pragma once

#include "SequenceTrack.h"

class SequenceTrackFactory
{
public:
    static SequenceTrack createCMaj7Arpeggio(int lengthInTicks, int beatDuration);
    static SequenceTrack createAm7Arpeggio(int lengthInTicks, int beatDuration);
    static SequenceTrack createKickSnare(int lengthInTicks, int beatDuration);
    static SequenceTrack createKickSnareWithHats(int lengthInTicks, int beatDuration);
    static SequenceTrack createBassLine(int lengthInTicks, int beatDuration);
    static SequenceTrack createMelodicBass(int lengthInTicks, int beatDuration);
    static SequenceTrack createHiHatPattern(int lengthInTicks, int beatDuration);
    static SequenceTrack createFourOnFloorKick(int lengthInTicks, int beatDuration);
    static SequenceTrack createSnareBackbeat(int lengthInTicks, int beatDuration);
    static SequenceTrack createPadChords(int lengthInTicks, int beatDuration);
    static SequenceTrack createSynthStabs(int lengthInTicks, int beatDuration);
    static SequenceTrack createClapBackbeat(int lengthInTicks, int beatDuration);
};
