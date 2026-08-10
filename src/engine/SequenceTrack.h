#pragma once

#include "ControlChange.h"
#include "StringHelper.h"
#include "MidiInOut.h"
#include "Note.h"
#include "ProgramChange.h"
#include "TimedEventList.h"

#include <cstdint>
#include <vector>

class SequenceTrack
{
public:
    SequenceTrack(const char* name = "", uint8_t channel = 0);

    const char* name() const noexcept { return name_; }
    uint8_t channel() const noexcept { return channel_; }

    void attachMidi(MidiInOut& midi);

    bool isMuted() const noexcept { return muted_; }
    void setMuted(bool muted);

    void setStartMuted() { startMuted_ = true; }

    void addNote(
        int startTick,
        int durationTicks,
        uint8_t note,
        uint8_t velocity);

    void addControlChange(
        int tick,
        uint8_t controller,
        uint8_t value);

    void addProgramChange(
        int tick,
        uint8_t program);

    void reset();
    void processTick(int position, bool loopWrap);
    void releaseActiveNotes();

private:
    struct ActiveNote
    {
        uint8_t note = 0;
        int remainingTicks = 0;
    };

    void startNote(const Note& note);
    void tickActiveNotes();

    StringHelper::NameBuffer name_ = {};
    uint8_t channel_ = 0;

    bool muted_ = false;
    bool startMuted_ = false;

    MidiInOut* midi_ = nullptr;
    std::vector<ActiveNote> activeNotes_;

    TimedEventList<Note> notes_;
    TimedEventList<ControlChange> controlChanges_;
    TimedEventList<ProgramChange> programChanges_;
};
