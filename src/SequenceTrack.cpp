#include "SequenceTrack.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

SequenceTrack::SequenceTrack(std::string name)
    : name_(std::move(name))
{
}

void SequenceTrack::attachMidi(MidiInOut& midi)
{
    midi_ = &midi;
}

MidiInOut& SequenceTrack::midi()
{
    if (!midi_) {
        throw std::logic_error("SequenceTrack: MIDI not attached");
    }

    return *midi_;
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

void SequenceTrack::addControlChange(
    int tick,
    uint8_t channel,
    uint8_t controller,
    uint8_t value)
{
    if (tick < 0) {
        throw std::invalid_argument("Invalid control change timing");
    }

    controlChanges_.push_back({ tick, channel, controller, value });
    sortControlChanges();
}

void SequenceTrack::addProgramChange(
    int tick,
    uint8_t channel,
    uint8_t program)
{
    if (tick < 0) {
        throw std::invalid_argument("Invalid program change timing");
    }

    programChanges_.push_back({ tick, channel, program });
    sortProgramChanges();
}

void SequenceTrack::sortNotes()
{
    std::sort(notes_.begin(), notes_.end(), [](const Note& a, const Note& b) {
        return a.tick < b.tick;
    });
}

void SequenceTrack::sortControlChanges()
{
    std::sort(controlChanges_.begin(), controlChanges_.end(),
        [](const ControlChange& a, const ControlChange& b) {
            return a.tick < b.tick;
        });
}

void SequenceTrack::sortProgramChanges()
{
    std::sort(programChanges_.begin(), programChanges_.end(),
        [](const ProgramChange& a, const ProgramChange& b) {
            return a.tick < b.tick;
        });
}

void SequenceTrack::reset()
{
    nextNoteIndex_ = 0;
    nextControlChangeIndex_ = 0;
    nextProgramChangeIndex_ = 0;
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
        //this might prevent from mute/unmute bug !
        nextNoteIndex_ = 0;
        nextControlChangeIndex_ = 0;
        nextProgramChangeIndex_ = 0;
    }
}

void SequenceTrack::startNote(const Note& note)
{
    midi().sendNoteOn(note.channel, note.note, note.velocity);
    activeNotes_.push_back({ note.channel, note.note, note.durationTicks });
}

void SequenceTrack::tickActiveNotes()
{
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();)
    {
        --it->remainingTicks;

        if (it->remainingTicks <= 0) {
            midi().sendNoteOff(it->channel, it->note, 0);
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

void SequenceTrack::releaseActiveNotes()
{
    for (const ActiveNote& activeNote : activeNotes_) {
        midi().sendNoteOff(activeNote.channel, activeNote.note, 0);
    }

    activeNotes_.clear();
}

void SequenceTrack::processProgramChanges(int position, bool loopWrap)
{
    if (loopWrap) {
        nextProgramChangeIndex_ = 0;
    }

    while (nextProgramChangeIndex_ < programChanges_.size()
           && programChanges_[nextProgramChangeIndex_].tick < position)
    {
        ++nextProgramChangeIndex_;
    }

    while (nextProgramChangeIndex_ < programChanges_.size()
           && programChanges_[nextProgramChangeIndex_].tick == position)
    {
        const ProgramChange& change = programChanges_[nextProgramChangeIndex_];
        midi().sendProgramChange(change.channel, change.program);
        ++nextProgramChangeIndex_;
    }
}

void SequenceTrack::processControlChanges(int position, bool loopWrap)
{
    if (loopWrap) {
        nextControlChangeIndex_ = 0;
    }

    while (nextControlChangeIndex_ < controlChanges_.size()
           && controlChanges_[nextControlChangeIndex_].tick < position)
    {
        ++nextControlChangeIndex_;
    }

    while (nextControlChangeIndex_ < controlChanges_.size()
           && controlChanges_[nextControlChangeIndex_].tick == position)
    {
        const ControlChange& change = controlChanges_[nextControlChangeIndex_];
        midi().sendControlChange(change.channel, change.controller, change.value);
        ++nextControlChangeIndex_;
    }
}

void SequenceTrack::processNotes(int position, bool loopWrap)
{
    if (loopWrap) {
        nextNoteIndex_ = 0;
    }

    while (nextNoteIndex_ < notes_.size() && notes_[nextNoteIndex_].tick < position) {
        ++nextNoteIndex_;
    }

    while (nextNoteIndex_ < notes_.size() && notes_[nextNoteIndex_].tick == position)
    {
        startNote(notes_[nextNoteIndex_]);
        ++nextNoteIndex_;
    }
}

void SequenceTrack::processTick(int position, bool loopWrap)
{
    if (!muted_) {
        processProgramChanges(position, loopWrap);
        processControlChanges(position, loopWrap);
        processNotes(position, loopWrap);
    }

    tickActiveNotes();
}
