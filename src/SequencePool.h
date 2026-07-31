#pragma once

#include "Sequence.h"
#include "VirtualMidiSender.h"

#include <vector>

class SequencePool
{
public:
    void add(Sequence sequence);
    void clear();

    std::size_t size() const noexcept { return sequences_.size(); }

    void reset();
    void processTick(VirtualMidiSender& sender, int tick);
    void allNotesOff(VirtualMidiSender& sender);

    const std::vector<Sequence>& sequences() const noexcept { return sequences_; }

private:
    std::vector<Sequence> sequences_;
};
