#include "SequencePool.h"

#include <iostream>
#include <utility>

void SequencePool::add(Sequence sequence)
{
    sequences_.push_back(std::move(sequence));
}

Sequence& SequencePool::current()
{
    return sequences_.at(currentIndex_);
}

const Sequence& SequencePool::current() const
{
    return sequences_.at(currentIndex_);
}

void SequencePool::resetCurrent()
{
    pendingSwitch_ = PendingSwitch::None;
    current().setLooping(true);
    current().reset();
}

void SequencePool::queueSwitch(PendingSwitch direction)
{
    if (sequences_.empty())
        return;

    if (pendingSwitch_ != PendingSwitch::None)
    {
        std::cout << "Already waiting to switch sequence.\n";
        return;
    }

    if (direction == PendingSwitch::Next && currentIndex_ + 1 >= sequences_.size())
    {
        std::cout << "Already on last sequence.\n";
        return;
    }

    if (direction == PendingSwitch::Previous && currentIndex_ == 0)
    {
        std::cout << "Already on first sequence.\n";
        return;
    }

    pendingSwitch_ = direction;
    current().setLooping(false);

    if (direction == PendingSwitch::Next)
        std::cout << "Next sequence queued — finishing current sequence...\n";
    else
        std::cout << "Previous sequence queued — finishing current sequence...\n";
}

void SequencePool::requestNext()
{
    queueSwitch(PendingSwitch::Next);
}

void SequencePool::requestPrevious()
{
    queueSwitch(PendingSwitch::Previous);
}

void SequencePool::processTick(VirtualMidiSender& sender, int tick)
{
    if (sequences_.empty())
        return;

    Sequence& sequence = current();
    sequence.processTick(sender, tick);

    if (pendingSwitch_ != PendingSwitch::None && sequence.position() >= sequence.lengthInTicks())
    {
        if (pendingSwitch_ == PendingSwitch::Next)
            advanceToNext(sender);
        else
            advanceToPrevious(sender);
    }
}

void SequencePool::allNotesOff(VirtualMidiSender& sender)
{
    for (Sequence& sequence : sequences_)
        sequence.allNotesOff(sender);
}

void SequencePool::advanceToNext(VirtualMidiSender& sender)
{
    current().allNotesOff(sender);

    pendingSwitch_ = PendingSwitch::None;
    ++currentIndex_;

    current().reset();
    current().setLooping(true);

    std::cout << "Switched to sequence " << currentIndex_ + 1 << " / " << sequences_.size()
              << " (" << current().trackCount() << " tracks)\n";
}

void SequencePool::advanceToPrevious(VirtualMidiSender& sender)
{
    current().allNotesOff(sender);

    pendingSwitch_ = PendingSwitch::None;
    --currentIndex_;

    current().reset();
    current().setLooping(true);

    std::cout << "Switched to sequence " << currentIndex_ + 1 << " / " << sequences_.size()
              << " (" << current().trackCount() << " tracks)\n";
}

SequencePool SequencePool::createDefault()
{
    SequencePool pool;
    pool.add(Sequence::createSequenceOne(4));
    pool.add(Sequence::createSequenceTwo(4));
    return pool;
}
