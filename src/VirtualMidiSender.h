#pragma once

#include "MidiInOut.h"

#include <RtMidi.h>

#include <cstdint>
#include <string>
#include <vector>

class VirtualMidiSender : public MidiInOut
{
public:
    explicit VirtualMidiSender(const std::string& portName);
    ~VirtualMidiSender() override;

    VirtualMidiSender(const VirtualMidiSender&) = delete;
    VirtualMidiSender& operator=(const VirtualMidiSender&) = delete;

    void sendClock() override;
    void sendStart() override;
    void sendStop() override;
    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override;
    void sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) override;
    void sendProgramChange(uint8_t channel, uint8_t program) override;

    const std::string& portName() const noexcept { return portName_; }

private:
    void sendRealtime(uint8_t status);
    void sendMessage(const std::vector<uint8_t>& message);

    std::string portName_;
    RtMidiOut midiOut_;
};
