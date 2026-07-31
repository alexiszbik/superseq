#include "Sequence.h"

#include <stdexcept>
#include <utility>

namespace
{
constexpr uint8_t kDrumChannel = 9; // MIDI channel 10
} // namespace

Sequence::Sequence(int barCount, int beatsPerBar, bool loop, int beatDuration)
    : barCount_(barCount)
    , beatsPerBar_(beatsPerBar)
    , beatDuration_(beatDuration)
    , looping_(loop)
{
    if (barCount_ <= 0) {
        throw std::invalid_argument("Sequence bar count must be positive");
    }

    if (beatsPerBar_ <= 0) {
        throw std::invalid_argument("Sequence beats per bar must be positive");
    }

    if (beatDuration_ <= 0) {
        throw std::invalid_argument("Sequence beat duration must be positive");
    }
}

int Sequence::lengthInTicks() const noexcept
{
    return barCount_ * beatsPerBar_ * beatDuration_;
}

void Sequence::addTrack(SequenceTrack track)
{
    tracks_.push_back(std::move(track));
}

void Sequence::clearTracks()
{
    tracks_.clear();
}

SequenceTrack& Sequence::track(std::size_t index)
{
    return tracks_.at(index);
}

const SequenceTrack& Sequence::track(std::size_t index) const
{
    return tracks_.at(index);
}

void Sequence::setTrackMuted(std::size_t index, bool muted, VirtualMidiSender& sender)
{
    tracks_.at(index).setMuted(muted, sender);
}

void Sequence::reset()
{
    for (SequenceTrack& track : tracks_) {
        track.reset();
    }
}

void Sequence::processTick(VirtualMidiSender& sender, int tick)
{
    const int length = lengthInTicks();
    if (length <= 0) {
        return;
    }

    if (!looping_ && tick >= length) {
        return;
    }

    const int position = looping_ ? (tick % length) : tick;
    const bool loopWrap = looping_ && position == 0 && tick > 0;

    for (SequenceTrack& track : tracks_) {
        track.processTick(sender, position, loopWrap);
    }
}

void Sequence::allNotesOff(VirtualMidiSender& sender)
{
    for (uint8_t channel = 0; channel < 16; ++channel)
    {
        for (int note = 0; note < 128; ++note) {
            sender.sendNoteOff(channel, static_cast<uint8_t>(note), 0);
        }
    }
}

// --- test factory func

SequenceTrack Sequence::createCMaj7Track(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Cmaj7 Arpeggio");

    const uint8_t notes[] = { 60, 64, 67, 71 };
    const int beatCount = lengthInTicks / beatDuration;
    const int noteDuration = beatDuration - 2;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        track.addNote(tick, noteDuration, notes[beat % 4], 100, 0, lengthInTicks);
    }

    return track;
}

SequenceTrack Sequence::createKickSnareTrack(int lengthInTicks, int beatDuration)
{
    SequenceTrack track("Kick/Snare");

    constexpr uint8_t kKickNote = 36;
    constexpr uint8_t kSnareNote = 37;
    constexpr int kHitDuration = 10;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        const bool isKickBeat = (beat % 4) == 0 || (beat % 4) == 2;
        const uint8_t note = isKickBeat ? kKickNote : kSnareNote;

        track.addNote(tick, kHitDuration, note, 127, kDrumChannel, lengthInTicks);
    }

    return track;
}

Sequence Sequence::createDemo(int barCount)
{
    Sequence sequence(barCount, 4, true);
    const int length = sequence.lengthInTicks();
    const int beatDuration = sequence.beatDuration();

    sequence.addTrack(createCMaj7Track(length, beatDuration));
    sequence.addTrack(createKickSnareTrack(length, beatDuration));

    return sequence;
}
