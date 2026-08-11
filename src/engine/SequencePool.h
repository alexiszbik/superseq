#pragma once

#include "Logger.h"
#include "MidiInOut.h"
#include "Sequence.h"
#include "Song.h"

#include <cstddef>
#include <vector>

class SequencePool
{
public:
    SequencePool(MidiInOut& midi, Logger& logger);

    void add(Song song);

    std::size_t songCount() const noexcept { return songs_.size(); }
    std::size_t sequenceCount() const noexcept;
    std::size_t currentSongIndex() const noexcept { return currentSongIndex_; }
    std::size_t currentSequenceIndex() const noexcept { return currentSequenceIndex_; }
    bool hasPendingSwitch() const noexcept { return pendingSwitch_ != PendingSwitch::None; }

    Song& currentSong();
    const Song& currentSong() const;
    Sequence& current();
    const Sequence& current() const;

    void resetCurrent();
    void requestNext(bool now = false);
    void requestPrevious(bool now = false);
    void processTick();
    void allNotesOff();

    static SequencePool createDefault(MidiInOut& midi, Logger& logger);

private:
    enum class PendingSwitch
    {
        None,
        Next,
        Previous
    };

    bool canAdvanceNext() const;
    bool canAdvancePrevious() const;
    void advanceToNext();
    void advanceToPrevious();
    void queueSwitch(PendingSwitch direction);
    void logCurrentSequenceSwitch();

    MidiInOut& midi_;
    Logger& logger_;
    std::vector<Song> songs_;
    std::size_t currentSongIndex_ = 0;
    std::size_t currentSequenceIndex_ = 0;
    PendingSwitch pendingSwitch_ = PendingSwitch::None;
};
