#pragma once

#include <cstdint>

class MidiInOut
{
public:
    virtual ~MidiInOut() = default;

    virtual void sendClock() = 0;
    virtual void sendStart() = 0;
    virtual void sendStop() = 0;
    virtual void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
    virtual void sendProgramChange(uint8_t channel, uint8_t program) = 0;
};
