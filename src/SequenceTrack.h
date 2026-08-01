#pragma once

#include "VirtualMidiSender.h"

#include <cstdint>
#include <string>
#include <vector>

class SequenceTrack
{
public:
    struct Event
    {
        int tick = 0;
        uint8_t channel = 0;
        uint8_t note = 0;
        uint8_t velocity = 0;
        bool noteOn = true;
    };

    explicit SequenceTrack(std::string name = {});

    const std::string& name() const noexcept { return name_; }
    const std::vector<Event>& events() const noexcept { return events_; }

    bool isMuted() const noexcept { return muted_; }
    void setMuted(bool muted, VirtualMidiSender& sender);

    void setStartMuted() { startMuted_ = true; }

    void addNote(
        int startTick,
        int durationTicks,
        uint8_t note,
        uint8_t velocity,
        uint8_t channel,
        int maxTick);

    void addEvent(const Event& event, int maxTick);

    void reset();
    void processTick(VirtualMidiSender& sender, int position, bool loopWrap);

private:
    struct ActiveNote
    {
        uint8_t channel = 0;
        uint8_t note = 0;
    };

    void sortEvents();
    void sendNoteOn(VirtualMidiSender& sender, const Event& event);
    void sendNoteOff(VirtualMidiSender& sender, const Event& event);
    void releaseActiveNotes(VirtualMidiSender& sender);
    void removeActiveNote(uint8_t channel, uint8_t note);

    std::string name_;
    bool muted_ = false;

    bool startMuted_ = false;
    std::vector<Event> events_;
    std::vector<ActiveNote> activeNotes_;
    std::size_t nextEventIndex_ = 0;
};
