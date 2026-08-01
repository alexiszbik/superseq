#include "SequenceTrackFactory.h"

#include <cmath>

namespace
{
constexpr uint8_t kDrumChannel = 9; // MIDI channel 10
constexpr int kDefaultHitDuration = 10;
} // namespace

SequenceTrack SequenceTrackFactory::createCMaj7Arpeggio(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Cmaj7 Arpeggio");

    const uint8_t notes[] = { 60, 64, 67, 71 };
    const int beatCount = lengthInTicks / beatDuration;
    const int noteDuration = beatDuration - 2;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        const int bar = static_cast<int>(std::floor(beat / 4.0));
        track.addNote(tick, noteDuration, static_cast<uint8_t>(notes[beat % 4] + 2 * bar), 100, 0, lengthInTicks);
    }

    track.setStartMuted();

    return track;
}

SequenceTrack SequenceTrackFactory::createAm7Arpeggio(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Am7 Arpeggio");

    const uint8_t notes[] = { 57, 60, 64, 67 }; // A C E G
    const int beatCount = lengthInTicks / beatDuration;
    const int noteDuration = beatDuration - 2;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        track.addNote(tick, noteDuration, notes[beat % 4], 95, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnare(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Kick/Snare");

    constexpr uint8_t kKickNote = 36;
    constexpr uint8_t kSnareNote = 38;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        const bool isKickBeat = (beat % 4) == 0 || (beat % 4) == 2;
        const uint8_t note = isKickBeat ? kKickNote : kSnareNote;

        track.addNote(tick, kDefaultHitDuration, note, 127, kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnareWithHats(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Kick/Snare + Hats");

    constexpr uint8_t kKickNote = 36;
    constexpr uint8_t kSnareNote = 37;
    constexpr uint8_t kHatNote = 42;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        const bool isSnare = (beat % 4) == 1 || (beat % 4) == 3;

        if (isSnare)
            track.addNote(tick, kDefaultHitDuration, kSnareNote, 127, kDrumChannel, lengthInTicks);

        track.addNote(tick, kDefaultHitDuration, kKickNote, 127, kDrumChannel, lengthInTicks);
    }

    const int sixteenthDuration = beatDuration / 4;
    const int sixteenthCount = lengthInTicks / sixteenthDuration;
    const int hatDuration = sixteenthDuration / 2;

    for (int sixteenth = 0; sixteenth < sixteenthCount; ++sixteenth)
    {
        const int velocityStep = (sixteenth + 1) % 4;
        const int tick = sixteenth * sixteenthDuration;
        const int velocity = (127 / 4) * (velocityStep + 1);

        track.addNote(tick, hatDuration, kHatNote, static_cast<uint8_t>(velocity), kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createBassLine(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Bassline");

    const int sixteenthDuration = beatDuration / 4;
    const int sixteenthCount = lengthInTicks / sixteenthDuration;
    const int noteLength = sixteenthDuration - 2;

    for (int sixteenth = 0; sixteenth < sixteenthCount; ++sixteenth)
    {
        const int tick = sixteenth * sixteenthDuration;
        const int bar = sixteenth / 16;
        const uint8_t note = (bar == 3) ? static_cast<uint8_t>(48) : static_cast<uint8_t>(36);

        track.addNote(tick, noteLength, note, 100, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createMelodicBass(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Melodic Bass");

    const uint8_t notes[] = { 36, 36, 43, 41, 38, 38, 43, 41 };
    const int eighthDuration = beatDuration / 2;
    const int eighthCount = lengthInTicks / eighthDuration;
    const int noteLength = eighthDuration - 2;

    for (int eighth = 0; eighth < eighthCount; ++eighth)
    {
        const int tick = eighth * eighthDuration;
        track.addNote(tick, noteLength, notes[eighth % 8], 110, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createHiHatPattern(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Hi-Hat");

    constexpr uint8_t kHatNote = 42;

    const int sixteenthDuration = beatDuration / 4;
    const int sixteenthCount = lengthInTicks / sixteenthDuration;
    const int noteLength = sixteenthDuration - 2;

    for (int sixteenth = 0; sixteenth < sixteenthCount; ++sixteenth)
    {
        if (sixteenth % 2 != 0)
            continue;

        const int tick = sixteenth * sixteenthDuration;
        const uint8_t velocity = static_cast<uint8_t>(80 + (sixteenth % 4) * 10);

        track.addNote(tick, noteLength, kHatNote, velocity, kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createFourOnFloorKick(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Four On Floor");

    constexpr uint8_t kKickNote = 36;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        track.addNote(tick, kDefaultHitDuration, kKickNote, 127, kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createSnareBackbeat(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Snare Backbeat");

    constexpr uint8_t kSnareNote = 37;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        if ((beat % 4) != 1 && (beat % 4) != 3)
            continue;

        const int tick = beat * beatDuration;
        track.addNote(tick, kDefaultHitDuration, kSnareNote, 120, kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createPadChords(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Pad Chords");

    const uint8_t barRoots[] = { 60, 65, 67, 60 }; // C F G C
    const int ticksPerBar = beatDuration * 4;
    const int barCount = lengthInTicks / ticksPerBar;
    const int chordDuration = ticksPerBar - 2;

    for (int bar = 0; bar < barCount; ++bar)
    {
        const int tick = bar * ticksPerBar;
        const uint8_t root = barRoots[bar % 4];

        track.addNote(tick, chordDuration, root, 70, 0, lengthInTicks);
        track.addNote(tick, chordDuration, static_cast<uint8_t>(root + 4), 70, 0, lengthInTicks);
        track.addNote(tick, chordDuration, static_cast<uint8_t>(root + 7), 70, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createSynthStabs(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Synth Stabs");

    const uint8_t notes[] = { 72, 74, 76, 77 };
    const int eighthDuration = beatDuration / 2;
    const int eighthCount = lengthInTicks / eighthDuration;
    const int noteLength = eighthDuration - 2;

    for (int eighth = 0; eighth < eighthCount; ++eighth)
    {
        if (eighth % 2 == 0)
            continue;

        const int tick = eighth * eighthDuration;
        track.addNote(tick, noteLength, notes[(eighth / 2) % 4], 105, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createClapBackbeat(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Clap");

    constexpr uint8_t kClapNote = 39;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        if ((beat % 4) != 1 && (beat % 4) != 3)
            continue;

        const int tick = beat * beatDuration;
        track.addNote(tick, kDefaultHitDuration, kClapNote, 115, kDrumChannel, lengthInTicks);
    }

    return track;
}
