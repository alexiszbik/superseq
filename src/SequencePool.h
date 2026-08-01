#pragma once

#include "Sequence.h"
#include "VirtualMidiSender.h"

#include <cstddef>
#include <vector>

class SequencePool
{
public:
    void add(Sequence sequence);

    std::size_t size() const noexcept { return sequences_.size(); }
    std::size_t currentIndex() const noexcept { return currentIndex_; }
    bool hasPendingSwitch() const noexcept { return pendingSwitch_ != PendingSwitch::None; }

    Sequence& current();
    const Sequence& current() const;

    void resetCurrent();
    void requestNext(VirtualMidiSender& sender, bool now = false);
    void requestPrevious(VirtualMidiSender& sender, bool now = false);
    void processTick(VirtualMidiSender& sender, int tick);
    void allNotesOff(VirtualMidiSender& sender);

    static SequencePool createDefault();

private:
    enum class PendingSwitch
    {
        None,
        Next,
        Previous
    };

    void advanceToNext(VirtualMidiSender& sender);
    void advanceToPrevious(VirtualMidiSender& sender);
    void queueSwitch(PendingSwitch direction);

    std::vector<Sequence> sequences_;
    std::size_t currentIndex_ = 0;
    PendingSwitch pendingSwitch_ = PendingSwitch::None;
};
