#pragma once

#include "ControlChange.h"
#include "Note.h"
#include "ProgramChange.h"
#include "VirtualMidiSender.h"

#include <cstdint>
#include <string>
#include <vector>

class SequenceTrack
{
public:
    explicit SequenceTrack(std::string name = {});

    const std::string& name() const noexcept { return name_; }

    bool isMuted() const noexcept { return muted_; }
    void setMuted(bool muted, VirtualMidiSender& sender);

    void setStartMuted() { startMuted_ = true; }

    void addNote(
        int startTick,
        int durationTicks,
        uint8_t note,
        uint8_t velocity,
        uint8_t channel);

    void addControlChange(
        int tick,
        uint8_t channel,
        uint8_t controller,
        uint8_t value);

    void addProgramChange(
        int tick,
        uint8_t channel,
        uint8_t program);

    void reset();
    void processTick(VirtualMidiSender& sender, int position, bool loopWrap);
    void releaseActiveNotes(VirtualMidiSender& sender);

private:
    struct ActiveNote
    {
        uint8_t channel = 0;
        uint8_t note = 0;
        int remainingTicks = 0;
    };

    void sortNotes();
    void sortControlChanges();
    void sortProgramChanges();

    void processProgramChanges(VirtualMidiSender& sender, int position, bool loopWrap);
    void processControlChanges(VirtualMidiSender& sender, int position, bool loopWrap);
    void processNotes(VirtualMidiSender& sender, int position, bool loopWrap);

    void startNote(VirtualMidiSender& sender, const Note& note);
    void tickActiveNotes(VirtualMidiSender& sender);

    std::string name_;
    
    bool muted_ = false;
    bool startMuted_ = false;

    std::vector<ActiveNote> activeNotes_;

    std::vector<Note> notes_;
    std::vector<ControlChange> controlChanges_;
    std::vector<ProgramChange> programChanges_;

    std::size_t nextNoteIndex_ = 0;
    std::size_t nextControlChangeIndex_ = 0;
    std::size_t nextProgramChangeIndex_ = 0;
};
