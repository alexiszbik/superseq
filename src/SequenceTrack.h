#pragma once

#include "VirtualMidiSender.h"

#include <cstdint>
#include <string>
#include <vector>

class SequenceTrack
{
public:
    struct Note
    {
        int tick = 0;
        int durationTicks = 0;
        uint8_t channel = 0;
        uint8_t note = 0;
        uint8_t velocity = 0;
    };

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
    void startNote(VirtualMidiSender& sender, const Note& note);
    void tickActiveNotes(VirtualMidiSender& sender);


    std::string name_;
    bool muted_ = false;

    bool startMuted_ = false;
    std::vector<Note> notes_;
    std::vector<ActiveNote> activeNotes_;
    std::size_t nextNoteIndex_ = 0;
};
