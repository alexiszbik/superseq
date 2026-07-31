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

    const std::string& portName() const noexcept { return portName_; }

private:
    void sendRealtime(uint8_t status);
    void sendMessage(const std::vector<uint8_t>& message);

    std::string portName_;
    RtMidiOut midiOut_;
};
