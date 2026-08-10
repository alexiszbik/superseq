#pragma once

#include "MidiInOut.h"
#include "SequenceTrack.h"
#include "StringHelper.h"
#include "Tick.h"

#include <cstddef>
#include <vector>

class Sequence
{
public:
    static constexpr int kTicksPerQuarterNote = 96;

    Sequence(
        const char* name,
        uint8_t barCount,
        uint8_t beatsPerBar = 4,
        uint8_t barLoop = 0);

    const char* name() const noexcept { return name_; }

    int barCount() const noexcept { return barCount_; }
    int beatsPerBar() const noexcept { return beatsPerBar_; }
    tick_t loopInPoint() const noexcept { return loopInPoint_; }
    int barLoop() const noexcept {
        return loopInPoint_ / (beatsPerBar_ * kTicksPerQuarterNote);
    }

    tick_t lengthInTicks() const noexcept;
    tick_t position() const noexcept { return position_; }

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

    uint8_t barCount_;
    uint8_t beatsPerBar_;

    tick_t loopInPoint_ = 0;
    tick_t position_ = 0;
    bool loopStartAfterWrap_ = false;
    std::vector<SequenceTrack> tracks_;

private:
    MidiInOut* midi_ = nullptr;
    StringHelper::NameBuffer name_ = {};
};
