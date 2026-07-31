#include "SequencePool.h"

void SequencePool::add(Sequence sequence)
{
    sequences_.push_back(std::move(sequence));
}

void SequencePool::clear()
{
    sequences_.clear();
}

void SequencePool::reset()
{
    for (Sequence& sequence : sequences_) {
        sequence.reset();
    }
}

void SequencePool::processTick(VirtualMidiSender& sender, int tick)
{
    for (Sequence& sequence : sequences_) {
        sequence.processTick(sender, tick);
    }
}

void SequencePool::allNotesOff(VirtualMidiSender& sender)
{
    for (Sequence& sequence : sequences_) {
        sequence.allNotesOff(sender);
    }
}
