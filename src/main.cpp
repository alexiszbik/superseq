#include "MidiClock.h"
#include "VirtualMidiSender.h"

#include <csignal>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
volatile std::sig_atomic_t gKeepRunning = 1;

void handleSignal(int)
{
    gKeepRunning = 0;
}

bool trySetTempo(MidiClock& clock, const std::string& argument)
{
    if (argument.empty())
    {
        std::cout << "Current tempo: " << clock.bpm() << " BPM\n";
        return true;
    }

    std::istringstream stream(argument);
    double bpm = 0.0;
    stream >> bpm;

    if (stream.fail() || !stream.eof())
    {
        std::cout << "Invalid tempo. Example: t 128\n";
        return true;
    }

    try
    {
        clock.setBpm(bpm);
        std::cout << "Tempo set to " << clock.bpm() << " BPM\n";
    }
    catch (const std::exception& error)
    {
        std::cout << error.what() << '\n';
    }

    return true;
}
} // namespace

int main()
{
    std::signal(SIGINT, handleSignal);

    constexpr auto kPortName = "ProtoSeq Virtual";
    constexpr double kDefaultBpm = 120.0;

    try
    {
        VirtualMidiSender sender(kPortName);
        MidiClock clock(sender);

        clock.setBpm(kDefaultBpm);

        std::cout << "Commands: [p]lay  [s]top  [t]empo <bpm>  [q]uit\n";

        while (gKeepRunning)
        {
            std::cout << "> ";
            std::string command;
            if (!std::getline(std::cin, command))
                break;

            if (command == "p" || command == "play")
                clock.play();
            else if (command == "s" || command == "stop")
                clock.stop();
            else if (command == "q" || command == "quit")
                break;
            else if (command == "t" || command == "tempo" || command == "bpm")
                trySetTempo(clock, "");
            else if (command.rfind("t ", 0) == 0 || command.rfind("tempo ", 0) == 0 || command.rfind("bpm ", 0) == 0)
            {
                const auto space = command.find(' ');
                trySetTempo(clock, command.substr(space + 1));
            }
            else if (!command.empty())
                std::cout << "Unknown command. Use p/s/t/q.\n";
        }

        clock.stop();
        std::cout << "Bye.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
