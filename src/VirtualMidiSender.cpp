#include "VirtualMidiSender.h"

#include <iostream>
#include <stdexcept>

VirtualMidiSender::VirtualMidiSender(const std::string& portName)
    : portName_(portName)
{
    try
    {
        midiOut_.openVirtualPort(portName_);
    }
    catch (const RtMidiError& error)
    {
        throw std::runtime_error(
            "Failed to create virtual MIDI port \"" + portName_ + "\": " + error.getMessage());
    }

    std::cout << "Virtual MIDI port created: \"" << portName_ << "\"\n";
    std::cout << "Connect a MIDI monitor or DAW to this port to verify output.\n";
}

VirtualMidiSender::~VirtualMidiSender()
{
    if (midiOut_.isPortOpen())
        midiOut_.closePort();
}

void VirtualMidiSender::sendClock()
{
    sendRealtime(0xF8);
}

void VirtualMidiSender::sendStart()
{
    sendRealtime(0xFA);
}

void VirtualMidiSender::sendStop()
{
    sendRealtime(0xFC);
}

void VirtualMidiSender::sendRealtime(uint8_t status)
{
    sendMessage({ status });
}

void VirtualMidiSender::sendMessage(const std::vector<uint8_t>& message)
{
    try
    {
        midiOut_.sendMessage(&message);
    }
    catch (const RtMidiError& error)
    {
        throw std::runtime_error("MIDI send failed: " + error.getMessage());
    }
}
