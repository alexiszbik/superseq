#include "Sequence.h"

#include "StringHelper.h"

#include <stdexcept>
#include <utility>

Sequence::Sequence(
    const char* name,
    uint8_t barCount,
    uint8_t beatsPerBar,
    uint8_t barLoop)
    : barCount_(barCount)
    , beatsPerBar_(beatsPerBar)
{
    StringHelper::copyName(name_, name);

    const uint32_t length = static_cast<uint32_t>(barCount_) * beatsPerBar_ * kTicksPerQuarterNote;
    if (!fitsInTickRange(length)) {
        throw std::invalid_argument("Sequence length exceeds maximum tick range");
    }

    const uint32_t loopInPoint = static_cast<uint32_t>(barLoop) * beatsPerBar_ * kTicksPerQuarterNote;
    loopInPoint_ = static_cast<tick_t>(loopInPoint);
}

tick_t Sequence::lengthInTicks() const noexcept
{
    return static_cast<tick_t>(barCount_ * beatsPerBar_ * kTicksPerQuarterNote);
}

void Sequence::attachMidi(MidiInOut& midi)
{
    midi_ = &midi;

    for (SequenceTrack& track : tracks_) {
        track.attachMidi(midi);
    }
}

void Sequence::addTrack(SequenceTrack track)
{
    if (midi_) {
        track.attachMidi(*midi_);
    }

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

void Sequence::setTrackMuted(std::size_t index, bool muted)
{
    tracks_.at(index).setMuted(muted);
}

void Sequence::reset()
{
    position_ = 0;
    loopStartAfterWrap_ = false;

    for (SequenceTrack& track : tracks_) {
        track.reset();
    }
}

void Sequence::processTick(bool wrapAtEnd)
{
    const tick_t length = lengthInTicks();
    if (length == 0) {
        return;
    }

    if (!wrapAtEnd && position_ >= length) {
        return;
    }

    const bool loopWrap = wrapAtEnd && loopStartAfterWrap_;
    loopStartAfterWrap_ = false;

    for (SequenceTrack& track : tracks_) {
        track.processTick(position_, loopWrap);
    }

    ++position_;
    if (position_ >= length) {
        if (wrapAtEnd) {
            position_ = loopInPoint_;
            loopStartAfterWrap_ = true;
        }
    }
}

void Sequence::allNotesOff()
{
    for (SequenceTrack& track : tracks_) {
        track.releaseActiveNotes();
    }
}
