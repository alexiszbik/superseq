#include "SequencePool.h"

#include "factories/SequenceFactory.h"

#include <cstdio>
#include <utility>

SequencePool::SequencePool(MidiInOut& midi, Logger& logger)
    : midi_(midi)
    , logger_(logger)
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
    if (sequences_.empty()) {
        return;
    }

    if (pendingSwitch_ == PendingSwitch::Next && direction == PendingSwitch::Previous) {
        logger_.info("Cancel next sequence.\n");
        pendingSwitch_ = PendingSwitch::None;
        return;
    }

    if (pendingSwitch_ == PendingSwitch::Previous && direction == PendingSwitch::Next) {
        logger_.info("Cancel previous sequence.\n");
        pendingSwitch_ = PendingSwitch::None;
        return;
    }

    if (pendingSwitch_ != PendingSwitch::None) {
        logger_.info("Already waiting to switch sequence.\n");
        return;
    }

    if (direction == PendingSwitch::Next && currentIndex_ + 1 >= sequences_.size()) {
        logger_.info("Already on last sequence.\n");
        return;
    }

    if (direction == PendingSwitch::Previous && currentIndex_ == 0) {
        logger_.info("Already on first sequence.\n");
        return;
    }

    pendingSwitch_ = direction;

    if (direction == PendingSwitch::Next) {
        logger_.info("Next sequence queued — finishing current sequence...\n");
    } else {
        logger_.info("Previous sequence queued — finishing current sequence...\n");
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

void SequencePool::processTick()
{
    if (sequences_.empty()) {
        return;
    }

    Sequence& sequence = current();
    const bool wrapAtEnd = pendingSwitch_ == PendingSwitch::None;
    sequence.processTick(wrapAtEnd);

    if (pendingSwitch_ != PendingSwitch::None && sequence.position() >= sequence.lengthInTicks()) {
        if (pendingSwitch_ == PendingSwitch::Next) {
            advanceToNext();
        } else {
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

void SequencePool::logCurrentSequenceSwitch()
{
    char buffer[128];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "Switched to sequence %zu / %zu — %s (%zu tracks)\n",
        currentIndex_ + 1,
        sequences_.size(),
        current().name(),
        current().trackCount());
    logger_.info(buffer);
}

void SequencePool::advanceToNext()
{
    if (currentIndex_ == (size() - 1)) {
        logger_.info("Already on last sequence.\n");
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;
    ++currentIndex_;

    current().reset();

    logCurrentSequenceSwitch();
}

void SequencePool::advanceToPrevious()
{
    if (currentIndex_ == 0) {
        logger_.info("Already on first sequence.\n");
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;
    --currentIndex_;

    current().reset();

    logCurrentSequenceSwitch();
}

SequencePool SequencePool::createDefault(MidiInOut& midi, Logger& logger)
{
    SequencePool pool(midi, logger);

    pool.add(SequenceFactory::createSequenceOne(4));
    pool.add(SequenceFactory::createSequenceTwo(4));
    pool.add(SequenceFactory::createSequenceThree(4));
    pool.add(SequenceFactory::createSequenceFour(4));
    pool.add(SequenceFactory::createSequenceFive(4));
    pool.add(SequenceFactory::createSequenceSix(4));
    pool.add(SequenceFactory::createSequenceSeven(2));

    return pool;
}
