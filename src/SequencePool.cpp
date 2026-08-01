#include "SequencePool.h"

#include "factories/SequenceFactory.h"

#include <iostream>
#include <utility>

SequencePool::SequencePool(MidiInOut& midi)
    : midi_(midi)
{
}

void SequencePool::add(Sequence sequence)
{
    sequence.attachMidi(midi_);
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

    if (direction == PendingSwitch::Next) {
        std::cout << "Next sequence queued — finishing current sequence...\n";
    }
    else {
        std::cout << "Previous sequence queued — finishing current sequence...\n";
    }
}

void SequencePool::requestNext(bool now)
{
    if (now) {
        advanceToNext();
    } else {
        queueSwitch(PendingSwitch::Next);
    }
}

void SequencePool::requestPrevious(bool now)
{
    if (now) {
        advanceToPrevious();
    } else {
        queueSwitch(PendingSwitch::Previous);
    }
}

void SequencePool::processTick(int tick)
{
    if (sequences_.empty()) {
        return;
    }

    Sequence& sequence = current();
    const bool wrapAtEnd = pendingSwitch_ == PendingSwitch::None;
    sequence.processTick(tick, wrapAtEnd);

    if (pendingSwitch_ != PendingSwitch::None && sequence.position() >= sequence.lengthInTicks())
    {
        if (pendingSwitch_ == PendingSwitch::Next) {
            advanceToNext();
        }
        else {
            advanceToPrevious(); 
        }
    }
}

void SequencePool::allNotesOff()
{
    for (Sequence& sequence : sequences_) {
        sequence.allNotesOff();
    }
}

void SequencePool::advanceToNext()
{
    if (currentIndex_ == (size() - 1)) {
        std::cout << "Already on last sequence.\n";
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;
    ++currentIndex_;

    current().reset();

    std::cout << "Switched to sequence " << currentIndex_ + 1 << " / " << sequences_.size()
              << " (" << current().trackCount() << " tracks)\n";
}

void SequencePool::advanceToPrevious()
{
    if (currentIndex_ == 0) {
        std::cout << "Already on first sequence.\n";
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;
    --currentIndex_;

    current().reset();

    std::cout << "Switched to sequence " << currentIndex_ + 1 << " / " << sequences_.size()
              << " (" << current().trackCount() << " tracks)\n";
}

SequencePool SequencePool::createDefault(MidiInOut& midi)
{
    SequencePool pool(midi);

    pool.add(SequenceFactory::createSequenceOne(4));
    pool.add(SequenceFactory::createSequenceTwo(4));
    pool.add(SequenceFactory::createSequenceThree(4));
    pool.add(SequenceFactory::createSequenceFour(4));
    pool.add(SequenceFactory::createSequenceFive(4));
    pool.add(SequenceFactory::createSequenceSix(4));
    pool.add(SequenceFactory::createSequenceSeven(2));

    return pool;
}
