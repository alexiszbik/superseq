#pragma once

#include <RtMidi.h>

#include <cstdint>
#include <string>
#include <vector>

class VirtualMidiSender
{
public:
    explicit VirtualMidiSender(const std::string& portName);
    ~VirtualMidiSender();

    VirtualMidiSender(const VirtualMidiSender&) = delete;
    VirtualMidiSender& operator=(const VirtualMidiSender&) = delete;

    void sendClock();
    void sendStart();
    void sendStop();

    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendControlChange(uint8_t channel, uint8_t controller, uint8_t value);
    void sendProgramChange(uint8_t channel, uint8_t program);

    const std::string& portName() const noexcept { return portName_; }

private:
    void sendRealtime(uint8_t status);
    void sendMessage(const std::vector<uint8_t>& message);

    std::string portName_;
    RtMidiOut midiOut_;
};
