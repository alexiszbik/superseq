#include "SequenceTrackFactory.h"

#include <cmath>
#include <vector>

struct SequenceDesc {
    uint8_t channel;

    std::vector<std::vector<uint8_t>> notes;
    std::vector<uint8_t> velocities;
    uint8_t rate;
};

namespace
{
constexpr uint8_t kDrumChannel = 9; // MIDI channel 10
constexpr int kDefaultHitDuration = 10;

constexpr int barDuration = 24 * 4;
} // namespace

inline void makeSequenceTrack(SequenceTrack& track, SequenceDesc desc, int lengthInTicks) {

    const int stepDuration = barDuration/desc.rate;
    const int noteDuration = stepDuration - 2; //to be modified

    const int seqSize = desc.notes.size();
    int seqIdx = 0;

    const int velSize = desc.velocities.size();

    for (int tick = 0; tick < lengthInTicks; tick += stepDuration)
    {
        const std::vector<uint8_t> notes = desc.notes[seqIdx];

        uint8_t velocity = 127;
        if (seqIdx < velSize) {
            velocity = desc.velocities[seqIdx];
        }

        for (auto& note : notes) {
            track.addNote(tick, noteDuration, note, velocity, desc.channel);
        }

        seqIdx++;
        if (seqIdx >= seqSize) seqIdx = 0;
    }
}

SequenceTrack SequenceTrackFactory::createFourOnFloorKick(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Four On Floor");

    SequenceDesc desc;
    desc.notes = {{36}, {36}, {36}, {36}};
    desc.rate = 4;
    desc.channel = kDrumChannel;

    makeSequenceTrack(track, desc, lengthInTicks);
    return track;
}

SequenceTrack SequenceTrackFactory::createCMaj7Arpeggio(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("CM7 arpeggios");

    SequenceDesc desc;
    desc.notes = {{60}, {64}, {67}, {71}};
    desc.rate = 4;
    desc.channel = 0;

    makeSequenceTrack(track, desc, lengthInTicks);
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
        track.addNote(tick, noteDuration, notes[beat % 4], 95, 0);
    }

    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnare(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Kick Snare");
    
    SequenceDesc desc;
    desc.notes = {{36}, {36,37}, {36}, {36,37}};
    desc.rate = 4;
    desc.channel = kDrumChannel;

    makeSequenceTrack(track, desc, lengthInTicks);
    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnareWithHats(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Hats");

    SequenceDesc desc;
    desc.notes = {{42}, {42}, {42}, {42}};
    desc.velocities = {27, 89, 127, 89};
    desc.rate = 16;
    desc.channel = kDrumChannel;

    makeSequenceTrack(track, desc, lengthInTicks);

    SequenceDesc desc2;
    desc2.notes = {{36}, {36,37}, {36}, {36,37}};
    desc2.rate = 4;
    desc2.channel = kDrumChannel;
    makeSequenceTrack(track, desc2, lengthInTicks);
    //track.setStartMuted();

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

        track.addNote(tick, noteLength, note, 100, 0);
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
        track.addNote(tick, noteLength, notes[eighth % 8], 110, 0);
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

        track.addNote(tick, noteLength, kHatNote, velocity, kDrumChannel);
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
        track.addNote(tick, kDefaultHitDuration, kSnareNote, 120, kDrumChannel);
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
        const int tick = bar * ticksPerBar + 48;
        const uint8_t root = barRoots[bar % 4];

        track.addNote(tick, chordDuration, root, 70, 0);
        track.addNote(tick, chordDuration, static_cast<uint8_t>(root + 4), 70, 0);
        track.addNote(tick, chordDuration, static_cast<uint8_t>(root + 7), 70, 0);
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
        track.addNote(tick, noteLength, notes[(eighth / 2) % 4], 105, 0);
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
        track.addNote(tick, kDefaultHitDuration, kClapNote, 115, kDrumChannel);
    }

    return track;
}
