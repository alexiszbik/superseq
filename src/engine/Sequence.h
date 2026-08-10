#pragma once

#include "MidiInOut.h"
#include "SequenceTrack.h"
#include "StringHelper.h"
#include "Tick.h"
#include "TransportPosition.h"

#include <cstddef>
#include <string>
#include <vector>

class Sequence
{
public:
    static constexpr int kTicksPerQuarterNote = 96;

    Sequence(
        const char* name,
        int barCount,
        int beatsPerBar = 4,
        int barLoop = 0);

    const char* name() const noexcept { return name_; }

    int barCount() const noexcept { return barCount_; }
    int beatsPerBar() const noexcept { return beatsPerBar_; }
    tick_t loopInPoint() const noexcept { return loopInPoint_; }
    int barLoop() const noexcept {
        return loopInPoint_ / (beatsPerBar_ * kTicksPerQuarterNote);
    }

    tick_t lengthInTicks() const noexcept;
    tick_t position() const noexcept { return position_; }

    TransportPosition transportPosition(tick_t tickIndex) const;
    std::string formatPlayhead() const;

    void attachMidi(MidiInOut& midi);

    void addTrack(SequenceTrack track);
    void clearTracks();

    std::size_t trackCount() const noexcept { return tracks_.size(); }
    SequenceTrack& track(std::size_t index);
    const SequenceTrack& track(std::size_t index) const;

    void setTrackMuted(std::size_t index, bool muted);

    void reset();
    void processTick(bool wrapAtEnd = true);
    void allNotesOff();

    int barCount_;
    int beatsPerBar_;

    tick_t loopInPoint_ = 0;
    tick_t position_ = 0;
    bool loopStartAfterWrap_ = false;
    std::vector<SequenceTrack> tracks_;

private:
    MidiInOut* midi_ = nullptr;
    StringHelper::NameBuffer name_ = {};
};
