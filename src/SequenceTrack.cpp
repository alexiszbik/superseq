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
    uint8_t channel,
    int maxTick)
{
    if (startTick < 0 || durationTicks <= 0)
        throw std::invalid_argument("Invalid note timing");

    addEvent({ startTick, channel, note, velocity, true }, maxTick);
    addEvent({ startTick + durationTicks, channel, note, 0, false }, maxTick);
}

void SequenceTrack::addEvent(const Event& event, int maxTick)
{
    if (event.tick < 0 || event.tick >= maxTick)
    {
        throw std::out_of_range(
            "Event tick " + std::to_string(event.tick) + " is outside sequence length "
            + std::to_string(maxTick));
    }

    events_.push_back(event);
    sortEvents();
}

void SequenceTrack::sortEvents()
{
    std::sort(events_.begin(), events_.end(), [](const Event& a, const Event& b) {
        if (a.tick != b.tick) {
            return a.tick < b.tick;
        }

        return a.noteOn && !b.noteOn;
    });
}

void SequenceTrack::reset()
{
    nextEventIndex_ = 0;
    activeNotes_.clear();
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

void SequenceTrack::sendNoteOn(VirtualMidiSender& sender, const Event& event)
{
    sender.sendNoteOn(event.channel, event.note, event.velocity);
    activeNotes_.push_back({ event.channel, event.note });
}

void SequenceTrack::sendNoteOff(VirtualMidiSender& sender, const Event& event)
{
    sender.sendNoteOff(event.channel, event.note, 0);
    removeActiveNote(event.channel, event.note);
}

void SequenceTrack::releaseActiveNotes(VirtualMidiSender& sender)
{
    for (const ActiveNote& activeNote : activeNotes_)
        sender.sendNoteOff(activeNote.channel, activeNote.note, 0);

    activeNotes_.clear();
}

void SequenceTrack::removeActiveNote(uint8_t channel, uint8_t note)
{
    for (auto it = activeNotes_.begin(); it != activeNotes_.end(); ++it)
    {
        if (it->channel == channel && it->note == note)
        {
            activeNotes_.erase(it);
            return;
        }
    }
}

void SequenceTrack::processTick(VirtualMidiSender& sender, int position, bool loopWrap)
{
    if (muted_) {
        return;
    }

    if (loopWrap) {
        nextEventIndex_ = 0;
    }

    while (nextEventIndex_ < events_.size() && events_[nextEventIndex_].tick < position) {
        ++nextEventIndex_;
    }

    while (nextEventIndex_ < events_.size() && events_[nextEventIndex_].tick == position)
    {
        const Event& event = events_[nextEventIndex_];

        if (event.noteOn) {
            sendNoteOn(sender, event);
        }
        else {
            sendNoteOff(sender, event);
        }

        ++nextEventIndex_;
    }
}
