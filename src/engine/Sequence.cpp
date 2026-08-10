#include "Sequence.h"

#include "StringHelper.h"

#include <stdexcept>
#include <string>
#include <utility>

Sequence::Sequence(
    const char* name,
    int barCount,
    int beatsPerBar,
    int barLoop)
    : barCount_(barCount)
    , beatsPerBar_(beatsPerBar)
{
    StringHelper::copyName(name_, name);

    if (barCount_ <= 0) {
        throw std::invalid_argument("Sequence bar count must be positive");
    }

    if (beatsPerBar_ <= 0) {
        throw std::invalid_argument("Sequence beats per bar must be positive");
    }

    if (barLoop < 0 || barLoop >= barCount_) {
        throw std::invalid_argument("barLoop must be between 0 and barCount - 1");
    }

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

TransportPosition Sequence::transportPosition(tick_t tickIndex) const
{
    return TransportPosition::fromTickIndex(
        tickIndex, beatsPerBar_, lengthInTicks());
}

std::string Sequence::formatPlayhead() const
{
    const tick_t tickIndex = position_ == 0 ? 0 : position_ - 1;
    return transportPosition(tickIndex).toString();
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
