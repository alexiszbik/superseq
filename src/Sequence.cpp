#include "Sequence.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
constexpr uint8_t kDrumChannel = 9; // MIDI channel 10
} // namespace

Sequence::Sequence(int barCount, int beatsPerBar, int barLoop, int beatDuration)
    : barCount_(barCount)
    , beatsPerBar_(beatsPerBar)
    , beatDuration_(beatDuration)
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

    if (barLoop < 0 || barLoop >= barCount_) {
        throw std::invalid_argument("barLoop must be between 0 and barCount - 1");
    }

    loopInPoint_ = barLoop * beatsPerBar_ * beatDuration_;
}

int Sequence::lengthInTicks() const noexcept
{
    return barCount_ * beatsPerBar_ * beatDuration_;
}

TransportPosition Sequence::transportPosition(int tickIndex) const
{
    return TransportPosition::fromTickIndex(
        tickIndex, beatsPerBar_, beatDuration_, lengthInTicks());
}

std::string Sequence::formatPlayhead() const
{
    const int tickIndex = position_ == 0 ? 0 : position_ - 1;
    return transportPosition(tickIndex).toString();
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
    position_ = 0;
    loopStartAfterWrap_ = false;

    for (SequenceTrack& track : tracks_) {
        track.reset();
    }
}

void Sequence::processTick(VirtualMidiSender& sender, int tick, bool wrapAtEnd)
{
    const int length = lengthInTicks();
    if (length <= 0) {
        return;
    }

    if (!wrapAtEnd && position_ >= length) {
        return;
    }

    const bool loopWrap = wrapAtEnd && loopStartAfterWrap_;
    loopStartAfterWrap_ = false;

    for (SequenceTrack& track : tracks_) {
        track.processTick(sender, position_, loopWrap);
    }

    ++position_;
    if (position_ >= length) {
        if (wrapAtEnd) {
            position_ = loopInPoint_;
            loopStartAfterWrap_ = true;
        }
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
        const int bar = floor(beat / 4);
        track.addNote(tick, noteDuration, notes[ beat % 4] + 2*bar, 100, 0, lengthInTicks);
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

SequenceTrack Sequence::createKickSnareTrack2(int lengthInTicks, int beatDuration) {
    SequenceTrack track("Kick/Snare 2");

    constexpr uint8_t kKickNote = 36;
    constexpr uint8_t kSnareNote = 37;
    constexpr uint8_t kHatNote = 42;
    constexpr int kHitDuration = 10;

    const int beatCount = lengthInTicks / beatDuration;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * beatDuration;
        const bool isSnare = (beat % 4) == 1 || (beat % 4) == 3;

        if (isSnare) {
            track.addNote(tick, kHitDuration, kSnareNote, 127, kDrumChannel, lengthInTicks);
        }

        track.addNote(tick, kHitDuration, kKickNote, 127, kDrumChannel, lengthInTicks);
    }

    const int sexteenthDuration = beatDuration / 4;
    const int sixteenthCount = lengthInTicks / sexteenthDuration;
    const int hatDuration = sexteenthDuration / 2;

    for (int sixteenth = 0; sixteenth < sixteenthCount; ++sixteenth)
    {
        const int velocityVariance = (sixteenth + 1) % 4;
        const int tick = sixteenth * sexteenthDuration;
        const int velocity = (127/4) * (velocityVariance + 1);
        track.addNote(tick, hatDuration, kHatNote, velocity, kDrumChannel, lengthInTicks);
    }

    return track;
}

SequenceTrack Sequence::bassLineTrack(int lengthInTicks, int beatDuration) {
    SequenceTrack track("bassline");

    const int sexteenthDuration = beatDuration / 4;
    const int sixteenthCount = lengthInTicks / sexteenthDuration;
    const int noteLength = sexteenthDuration - 2;

    for (int sixteenth = 0; sixteenth < sixteenthCount; ++sixteenth)
    {
        const int tick = sixteenth * sexteenthDuration;
        const int bar = sixteenth / 16;
        track.addNote(tick, noteLength, (bar == 3) ? 48 : 36, 100, 0, lengthInTicks);
    }

    return track;
}

void Sequence::populateSequenceOne(Sequence& sequence, int lengthInTicks, int beatDuration)
{
    sequence.addTrack(createCMaj7Track(lengthInTicks, beatDuration));
    sequence.addTrack(createKickSnareTrack(lengthInTicks, beatDuration));
}

void Sequence::populateSequenceTwo(Sequence& sequence, int lengthInTicks, int beatDuration)
{
    sequence.addTrack(createKickSnareTrack2(lengthInTicks, beatDuration));
    sequence.addTrack(bassLineTrack(lengthInTicks, beatDuration));
}

Sequence Sequence::createSequenceOne(int barCount)
{
    Sequence sequence(barCount, 4, 2);
    const int length = sequence.lengthInTicks();
    const int beatDuration = sequence.beatDuration();

    populateSequenceOne(sequence, length, beatDuration);

    return sequence;
}

Sequence Sequence::createSequenceTwo(int barCount)
{
    Sequence sequence(barCount, 4);
    const int length = sequence.lengthInTicks();
    const int beatDuration = sequence.beatDuration();

    populateSequenceTwo(sequence, length, beatDuration);

    return sequence;
}
