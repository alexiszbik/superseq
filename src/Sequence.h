#pragma once

#include "VirtualMidiSender.h"

#include <cstdint>
#include <vector>

class Sequence
{
public:
    static constexpr int kTicksPerQuarterNote = 24;

    struct Event
    {
        int tick = 0;
        uint8_t channel = 0;
        uint8_t note = 0;
        uint8_t velocity = 0;
        bool noteOn = true;
    };

    Sequence(int barCount, int beatsPerBar = 4, bool loop = true, int beatDuration = kTicksPerQuarterNote);

    int barCount() const noexcept { return barCount_; }
    int beatsPerBar() const noexcept { return beatsPerBar_; }
    bool isLooping() const noexcept { return looping_; }

    void setLooping(bool loop) noexcept { looping_ = loop; }

    int lengthInTicks() const noexcept;
    const std::vector<Event>& events() const noexcept { return events_; }

    void addNote(int startTick, int durationTicks, uint8_t note, uint8_t velocity, uint8_t channel = 0);
    void addEvent(const Event& event);

    void reset();
    void processTick(VirtualMidiSender& sender, int tick);
    void allNotesOff(VirtualMidiSender& sender);

    static Sequence createCMaj7Arpeggio(int barCount = 4);

private:
    void sortEvents();

    int barCount_;
    int beatsPerBar_;
    bool looping_;
    int beatDuration_;

    std::vector<Event> events_;
    std::size_t nextEventIndex_ = 0;
};
