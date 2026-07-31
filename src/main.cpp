#include "VirtualMidiSender.h"

#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

namespace
{
volatile std::sig_atomic_t gKeepRunning = 1;

void handleSignal(int)
{
    gKeepRunning = 0;
}

const char* noteName(uint8_t note)
{
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return names[note % 12];
}

int noteOctave(uint8_t note)
{
    return static_cast<int>(note / 12) - 1;
}
} // namespace

int main()
{
    std::signal(SIGINT, handleSignal);

    constexpr auto kPortName = "ProtoSeq Virtual";
    constexpr auto kInterval = std::chrono::milliseconds(500);
    constexpr uint8_t kChannel = 0;
    constexpr uint8_t kVelocity = 100;
    constexpr uint8_t kNoteDurationMs = 200;

    const std::vector<uint8_t> sequence = { 60, 62, 64, 65, 67, 69, 71, 72 }; // C major scale

    try
    {
        VirtualMidiSender sender(kPortName);

        std::cout << "Sending notes every " << kInterval.count() << " ms. Press Ctrl+C to stop.\n";

        std::size_t index = 0;

        while (gKeepRunning)
        {
            const uint8_t note = sequence[index % sequence.size()];
            ++index;

            sender.sendNoteOn(kChannel, note, kVelocity);
            std::cout << "Note ON  " << noteName(note) << noteOctave(note)
                      << " (midi " << static_cast<int>(note) << ")\n";

            std::this_thread::sleep_for(std::chrono::milliseconds(kNoteDurationMs));
            sender.sendNoteOff(kChannel, note, kVelocity);
            std::cout << "Note OFF " << noteName(note) << noteOctave(note) << "\n";

            std::this_thread::sleep_for(kInterval - std::chrono::milliseconds(kNoteDurationMs));
        }

        std::cout << "Stopped.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
