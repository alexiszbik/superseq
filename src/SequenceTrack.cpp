#include "SequenceTrack.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

SequenceTrack::SequenceTrack(std::string name)
    : name_(std::move(name))
{
}

void SequenceTrack::addNote(
    int startTick,
    int durationTicks,
    uint8_t note,
    uint8_t velocity,
    uint8_t channel)
{
    if (startTick < 0 || durationTicks <= 0) {
        throw std::invalid_argument("Invalid note timing");
    }

    notes_.push_back({ startTick, durationTicks, channel, note, velocity });
    sortNotes();
}

void SequenceTrack::sortNotes()
{
    std::sort(notes_.begin(), notes_.end(), [](const Note& a, const Note& b) {
        return a.tick < b.tick;
    });
}

void SequenceTrack::reset()
{
    nextNoteIndex_ = 0;
    activeNotes_.clear();
    muted_ = startMuted_;
}

void SequenceTrack::setMuted(bool muted, VirtualMidiSender& sender)
{
    if (muted == muted_) {
        return;
    }

    muted_ = muted;

    if (muted) {
        releaseActiveNotes(sender);
    }
}

void SequenceTrack::startNote(VirtualMidiSender& sender, const Note& note)
{
    sender.sendNoteOn(note.channel, note.note, note.velocity);
    activeNotes_.push_back({ note.channel, note.note, note.durationTicks });
}

void SequenceTrack::tickActiveNotes(VirtualMidiSender& sender)
{
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();)
    {
        --it->remainingTicks;

        if (it->remainingTicks <= 0) {
            sender.sendNoteOff(it->channel, it->note, 0);
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

void SequenceTrack::releaseActiveNotes(VirtualMidiSender& sender)
{
    for (const ActiveNote& activeNote : activeNotes_) {
        sender.sendNoteOff(activeNote.channel, activeNote.note, 0);
    }

    activeNotes_.clear();
}

void SequenceTrack::processTick(VirtualMidiSender& sender, int position, bool loopWrap)
{
    if (muted_) {
        return;
    }

    if (loopWrap) {
        nextNoteIndex_ = 0;
    }

    while (nextNoteIndex_ < notes_.size() && notes_[nextNoteIndex_].tick < position) {
        ++nextNoteIndex_;
    }

    while (nextNoteIndex_ < notes_.size() && notes_[nextNoteIndex_].tick == position)
    {
        startNote(sender, notes_[nextNoteIndex_]);
        ++nextNoteIndex_;
    }

    tickActiveNotes(sender);
}
