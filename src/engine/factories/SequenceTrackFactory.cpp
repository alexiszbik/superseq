#include "SequenceTrackFactory.h"

#include "MidiChannel.h"
#include "Sequence.h"
#include "Tick.h"

#include <cstdint>
#include <vector>

namespace
{

struct SequenceDesc
{
    std::vector<std::vector<uint8_t>> notes;
    std::vector<uint8_t> velocities;
    std::vector<uint8_t> durations;
    uint8_t rate = 4;
};

void makeSequenceTrack(
    SequenceTrack& track,
    const SequenceDesc& desc,
    tick_t lengthInTicks)
{
    const int barDuration = Sequence::kTicksPerQuarterNote * 4;
    const int stepDuration = barDuration / desc.rate;

    const int seqSize = static_cast<int>(desc.notes.size());
    int seqIdx = 0;

    const int velSize = static_cast<int>(desc.velocities.size());
    int velIdx = 0;

    const int durationSize = static_cast<int>(desc.durations.size());
    int durIdx = 0;

    for (tick_t tick = 0; tick < lengthInTicks; tick = static_cast<tick_t>(tick + stepDuration))
    {
        int noteDuration = stepDuration;
        const std::vector<uint8_t>& stepNotes = desc.notes[seqIdx];

        uint8_t velocity = 127;
        if (velIdx < velSize) {
            velocity = desc.velocities[velIdx];
        }

        if (durIdx < durationSize) {
            noteDuration *= desc.durations[durIdx];
        }

        bool noteExists = false;

        for (uint8_t note : stepNotes) {
            track.addNote(tick, noteDuration, note, velocity);
            noteExists = true;
        }

        seqIdx = (seqIdx + 1) % seqSize;

        if (noteExists) {
            if (velSize > 0) {
                velIdx = (velIdx + 1) % velSize;
            }

            if (durationSize > 0) {
                durIdx = (durIdx + 1) % durationSize;
            }
        }
    }
}

void makeAutomationTrack(
    SequenceTrack& track,
    tick_t startInTicks,
    tick_t endInTicks,
    uint8_t controller,
    uint8_t startValue,
    uint8_t endValue)
{
    if (startInTicks > endInTicks) {
        return;
    }

    if (startInTicks == endInTicks) {
        track.addControlChange(startInTicks, controller, endValue);
        return;
    }

    const tick_t duration = endInTicks - startInTicks;
    uint8_t lastSentValue = startValue;

    track.addControlChange(startInTicks, controller, startValue);

    for (tick_t tick = startInTicks + 1; tick < endInTicks; ++tick) {
        const tick_t delta = tick - startInTicks;
        const int range = static_cast<int>(endValue) - startValue;

        const uint8_t value = static_cast<uint8_t>(
            startValue + range * static_cast<int>(delta) / static_cast<int>(duration));

        if (value != lastSentValue) {
            track.addControlChange(tick, controller, value);
            lastSentValue = value;
        }
    }

    track.addControlChange(endInTicks, controller, endValue);
}

} // namespace

SequenceTrack SequenceTrackFactory::createFourOnFloorKick(tick_t lengthInTicks)
{
    SequenceTrack track("Four On Floor", MidiChannel::kDrums);

    SequenceDesc desc;
    desc.notes = {{36}, {36}, {36}, {36}};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createCMaj7Arpeggio(tick_t lengthInTicks)
{
    SequenceTrack track("Cmaj7 Arpeggio", MidiChannel::kModularA);

    SequenceDesc desc;
    desc.notes = {{60}, {64}, {67}, {71}};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    makeAutomationTrack(track, 0, 96 * 4, 10, 0, 127);
    makeAutomationTrack(track, 96 * 4, 96 * 8, 10, 0, 127);
    makeAutomationTrack(track, 96 * 8, 96 * 12, 10, 0, 127);
    makeAutomationTrack(track, 96 * 12, 96 * 16, 10, 0, 127);

    return track;
}

SequenceTrack SequenceTrackFactory::createAm7Arpeggio(tick_t lengthInTicks)
{
    SequenceTrack track("Am7 Arpeggio", MidiChannel::kModularA);

    SequenceDesc desc;
    desc.notes = {{57}, {60}, {64}, {67}};
    desc.velocities = {95};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnare(tick_t lengthInTicks)
{
    SequenceTrack track("Kick/Snare", MidiChannel::kDrums);

    SequenceDesc desc;
    desc.notes = {{36}, {36, 38}, {36}, {36, 38}};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createKickSnareWithHats(tick_t lengthInTicks)
{
    SequenceTrack track("Kick/Snare + Hats", MidiChannel::kDrums);

    SequenceDesc hats;
    hats.notes = {{42}};
    hats.velocities = {32, 64, 96, 127, 32, 64, 96, 127};
    hats.rate = 16;
    makeSequenceTrack(track, hats, lengthInTicks);

    SequenceDesc drums;
    drums.notes = {
        {36}, {}, {36, 37}, {}, {36}, {}, {36, 37}, {},
        {36}, {}, {36, 37}, {}, {36}, {37}};
    drums.rate = 8;
    makeSequenceTrack(track, drums, lengthInTicks);

    track.setStartMuted();

    return track;
}

SequenceTrack SequenceTrackFactory::createBassLine(tick_t lengthInTicks)
{
    SequenceTrack track("Bassline", MidiChannel::kModularA);

    SequenceDesc desc;
    desc.notes = {
        {36}, {36}, {36}, {36},
        {36}, {36}, {36}, {36},
        {36}, {36}, {36}, {36},
        {48}, {48}, {48}, {48},
    };
    desc.velocities = {100};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createMelodicBass(tick_t lengthInTicks)
{
    SequenceTrack track("Melodic Bass", 0);

    SequenceDesc desc;
    desc.notes = {{36}, {36}, {43}, {41}, {38}, {38}, {43}, {41}};
    desc.velocities = {110};
    desc.rate = 8;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createHiHatPattern(tick_t lengthInTicks)
{
    SequenceTrack track("Hi-Hat", MidiChannel::kDrums);

    SequenceDesc desc;
    desc.notes = {
        {42}, {}, {42}, {},
        {42}, {46}, {42}, {},
        {42}, {}, {42}, {46},
        {42}, {}, {42}, {},
    };
    desc.velocities = {
        100, 80,
        100, 80, 80,
        100, 80, 100,
        100, 80};
    desc.rate = 16;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createSnareBackbeat(tick_t lengthInTicks)
{
    SequenceTrack track("Snare Backbeat", MidiChannel::kDrums);

    SequenceDesc desc;
    desc.notes = {{}, {37}, {}, {37}};
    desc.velocities = {120};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createPadChords(tick_t lengthInTicks)
{
    SequenceTrack track("Pad Chords", MidiChannel::kModularA);

    SequenceDesc desc;
    desc.notes = {
        {}, {60, 64, 67}, {}, {},
        {}, {65, 69, 72}, {}, {},
        {}, {67, 72, 75}, {}, {},
        {}, {60, 64, 67}, {}, {},
    };
    desc.velocities = {70};
    desc.durations = {3};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createSynthStabs(tick_t lengthInTicks)
{
    SequenceTrack track("Synth Stabs", MidiChannel::kModularA);

    SequenceDesc desc;
    desc.notes = {{}, {72}, {}, {74}, {}, {76}, {}, {77}};
    desc.velocities = {105};
    desc.rate = 8;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}

SequenceTrack SequenceTrackFactory::createClapBackbeat(tick_t lengthInTicks)
{
    SequenceTrack track("Clap", MidiChannel::kDrums);

    SequenceDesc desc;
    desc.notes = {{}, {39}, {}, {39}};
    desc.velocities = {115};
    desc.rate = 4;
    makeSequenceTrack(track, desc, lengthInTicks);

    return track;
}
