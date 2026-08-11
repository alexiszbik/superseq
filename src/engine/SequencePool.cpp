#include "SequencePool.h"

#include "factories/SequenceFactory.h"

#include <cstdio>
#include <utility>

SequencePool::SequencePool(MidiInOut& midi, Logger& logger)
    : midi_(midi)
    , logger_(logger)
{
}

void SequencePool::add(Song song)
{
    song.attachMidi(midi_);
    songs_.push_back(std::move(song));
}

std::size_t SequencePool::sequenceCount() const noexcept
{
    std::size_t count = 0;
    for (const Song& song : songs_) {
        count += song.size();
    }
    return count;
}

Song& SequencePool::currentSong()
{
    return songs_.at(currentSongIndex_);
}

const Song& SequencePool::currentSong() const
{
    return songs_.at(currentSongIndex_);
}

Sequence& SequencePool::current()
{
    return currentSong().sequence(currentSequenceIndex_);
}

const Sequence& SequencePool::current() const
{
    return currentSong().sequence(currentSequenceIndex_);
}

void SequencePool::resetCurrent()
{
    pendingSwitch_ = PendingSwitch::None;
    current().reset();
}

bool SequencePool::canAdvanceNext() const
{
    if (songs_.empty()) {
        return false;
    }

    if (currentSequenceIndex_ + 1 < currentSong().size()) {
        return true;
    }

    return currentSongIndex_ + 1 < songs_.size()
        && songs_[currentSongIndex_ + 1].size() > 0;
}

bool SequencePool::canAdvancePrevious() const
{
    if (songs_.empty()) {
        return false;
    }

    if (currentSequenceIndex_ > 0) {
        return true;
    }

    return currentSongIndex_ > 0
        && songs_[currentSongIndex_ - 1].size() > 0;
}

void SequencePool::queueSwitch(PendingSwitch direction)
{
    if (songs_.empty()) {
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

    if (direction == PendingSwitch::Next && !canAdvanceNext()) {
        logger_.info("Already on last sequence.\n");
        return;
    }

    if (direction == PendingSwitch::Previous && !canAdvancePrevious()) {
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
    if (songs_.empty()) {
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
    for (Song& song : songs_) {
        song.allNotesOff();
    }
}

void SequencePool::logCurrentSequenceSwitch()
{
    char buffer[192];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "Switched to song %zu / %zu — sequence %zu / %zu — %s (%zu tracks)\n",
        currentSongIndex_ + 1,
        songs_.size(),
        currentSequenceIndex_ + 1,
        currentSong().size(),
        current().name(),
        current().trackCount());
    logger_.info(buffer);
}

void SequencePool::advanceToNext()
{
    if (!canAdvanceNext()) {
        logger_.info("Already on last sequence.\n");
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;

    if (currentSequenceIndex_ + 1 < currentSong().size()) {
        ++currentSequenceIndex_;
    } else {
        ++currentSongIndex_;
        currentSequenceIndex_ = 0;
    }

    current().reset();

    logCurrentSequenceSwitch();
}

void SequencePool::advanceToPrevious()
{
    if (!canAdvancePrevious()) {
        logger_.info("Already on first sequence.\n");
        return;
    }

    current().allNotesOff();

    pendingSwitch_ = PendingSwitch::None;

    if (currentSequenceIndex_ > 0) {
        --currentSequenceIndex_;
    } else {
        --currentSongIndex_;
        currentSequenceIndex_ = currentSong().size() - 1;
    }

    current().reset();

    logCurrentSequenceSwitch();
}

SequencePool SequencePool::createDefault(MidiInOut& midi, Logger& logger)
{
    SequencePool pool(midi, logger);

    Song intro("Intro");
    intro.add(SequenceFactory::createSequenceOne(4));
    intro.add(SequenceFactory::createSequenceTwo(4));
    pool.add(std::move(intro));

    Song mainSong("Main");
    mainSong.add(SequenceFactory::createSequenceThree(4));
    mainSong.add(SequenceFactory::createSequenceFour(4));
    mainSong.add(SequenceFactory::createSequenceFive(4));
    pool.add(std::move(mainSong));

    Song outro("Outro");
    outro.add(SequenceFactory::createSequenceSix(4));
    outro.add(SequenceFactory::createSequenceSeven(2));
    pool.add(std::move(outro));

    return pool;
}
