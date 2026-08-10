#include "SequenceTrack.h"

#include "StringHelper.h"

#include <stdexcept>

SequenceTrack::SequenceTrack(const char* name, uint8_t channel)
    : channel_(channel)
{
    StringHelper::copyName(name_, name);
}

void SequenceTrack::attachMidi(MidiInOut& midi)
{
    midi_ = &midi;
}

void SequenceTrack::addNote(
    tick_t startTick,
    tick_t durationTicks,
    uint8_t note,
    uint8_t velocity)
{
    if (durationTicks == 0) {
        throw std::invalid_argument("Invalid note timing");
    }

    if (!fitsInTickRange(static_cast<uint32_t>(startTick) + durationTicks)) {
        throw std::invalid_argument("Note exceeds maximum tick range");
    }

    notes_.add({ startTick, durationTicks, note, velocity });
}

void SequenceTrack::addControlChange(
    tick_t tick,
    uint8_t controller,
    uint8_t value)
{
    controlChanges_.add({ tick, controller, value });
}

void SequenceTrack::addProgramChange(
    tick_t tick,
    uint8_t program)
{
    programChanges_.add({ tick, program });
}

void SequenceTrack::reset()
{
    notes_.reset();
    controlChanges_.reset();
    programChanges_.reset();
    activeNotes_.clear();
    muted_ = startMuted_;
}

void SequenceTrack::setMuted(bool muted)
{
    if (muted == muted_) {
        return;
    }

    muted_ = muted;

    if (muted) {
        releaseActiveNotes();
        notes_.reset();
        controlChanges_.reset();
        programChanges_.reset();
    }
}

void SequenceTrack::startNote(const Note& note)
{
    midi_->sendNoteOn(channel_, note.note, note.velocity);
    activeNotes_.push_back({ note.note, note.durationTicks });
}

void SequenceTrack::tickActiveNotes()
{
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();)
    {
        --it->remainingTicks;

        if (it->remainingTicks == 0) {
            midi_->sendNoteOff(channel_, it->note, 0);
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

void SequenceTrack::releaseActiveNotes()
{
    for (const ActiveNote& activeNote : activeNotes_) {
        midi_->sendNoteOff(channel_, activeNote.note, 0);
    }

    activeNotes_.clear();
}

void SequenceTrack::processTick(tick_t position, bool loopWrap)
{
    if (!muted_) {
        programChanges_.process(position, loopWrap, [this](const ProgramChange& change) {
            midi_->sendProgramChange(channel_, change.program);
        });

        controlChanges_.process(position, loopWrap, [this](const ControlChange& change) {
            midi_->sendControlChange(channel_, change.controller, change.value);
        });

        notes_.process(position, loopWrap, [this](const Note& note) {
            startNote(note);
        });
    }

    tickActiveNotes();
}
