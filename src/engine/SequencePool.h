#pragma once

#include "MidiInOut.h"
#include "Sequence.h"

#include <cstddef>
#include <vector>

class SequencePool
{
public:
    explicit SequencePool(MidiInOut& midi);

    void add(Sequence sequence);

    std::size_t size() const noexcept { return sequences_.size(); }
    std::size_t currentIndex() const noexcept { return currentIndex_; }
    bool hasPendingSwitch() const noexcept { return pendingSwitch_ != PendingSwitch::None; }

    Sequence& current();
    const Sequence& current() const;

    void resetCurrent();
    void requestNext(bool now = false);
    void requestPrevious(bool now = false);
    void processTick();
    void allNotesOff();

    static SequencePool createDefault(MidiInOut& midi);

private:
    enum class PendingSwitch
    {
        None,
        Next,
        Previous
    };

    void advanceToNext();
    void advanceToPrevious();
    void queueSwitch(PendingSwitch direction);

    MidiInOut& midi_;
    std::vector<Sequence> sequences_;
    std::size_t currentIndex_ = 0;
    PendingSwitch pendingSwitch_ = PendingSwitch::None;
};
