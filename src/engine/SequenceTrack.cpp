#include "SequenceTrack.h"

#include "StringHelper.h"

#include <stdexcept>

SequenceTrack::SequenceTrack(const char* name)
{
    StringHelper::copyName(name_, name);
}

void SequenceTrack::attachMidi(MidiInOut& midi)
{
    midi_ = &midi;
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

    notes_.add({ startTick, durationTicks, channel, note, velocity });
}

void SequenceTrack::addControlChange(
    int tick,
    uint8_t channel,
    uint8_t controller,
    uint8_t value)
{
    if (tick < 0) {
        throw std::invalid_argument("Invalid control change timing");
    }

    controlChanges_.add({ tick, channel, controller, value });
}

void SequenceTrack::addProgramChange(
    int tick,
    uint8_t channel,
    uint8_t program)
{
    if (tick < 0) {
        throw std::invalid_argument("Invalid program change timing");
    }

    programChanges_.add({ tick, channel, program });
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
    midi_->sendNoteOn(note.channel, note.note, note.velocity);
    activeNotes_.push_back({ note.channel, note.note, note.durationTicks });
}

void SequenceTrack::tickActiveNotes()
{
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();)
    {
        --it->remainingTicks;

        if (it->remainingTicks <= 0) {
            midi_->sendNoteOff(it->channel, it->note, 0);
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

void SequenceTrack::releaseActiveNotes()
{
    for (const ActiveNote& activeNote : activeNotes_) {
        midi_->sendNoteOff(activeNote.channel, activeNote.note, 0);
    }

    activeNotes_.clear();
}

void SequenceTrack::processTick(int position, bool loopWrap)
{
    if (!muted_) {
        programChanges_.process(position, loopWrap, [this](const ProgramChange& change) {
            midi_->sendProgramChange(change.channel, change.program);
        });

        controlChanges_.process(position, loopWrap, [this](const ControlChange& change) {
            midi_->sendControlChange(change.channel, change.controller, change.value);
        });

        notes_.process(position, loopWrap, [this](const Note& note) {
            startNote(note);
        });
    }

    tickActiveNotes();
}
