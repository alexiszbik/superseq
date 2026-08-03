#include "MidiClock.h"
#include "Sequence.h"
#include "SequencePool.h"
#include "TransportPosition.h"
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

void printTracks(const Sequence& sequence)
{
    for (std::size_t i = 0; i < sequence.trackCount(); ++i)
    {
        const SequenceTrack& track = sequence.track(i);
        std::cout << "  [" << i << "] " << track.name()
                  << (track.isMuted() ? " (muted)" : "") << "\n";
    }
}

void printPoolStatus(const SequencePool& pool)
{
    std::cout << "Sequence " << pool.currentIndex() + 1 << " / " << pool.size() << "\n";
    printTracks(pool.current());
}

bool tryMuteTrack(SequencePool& pool, const std::string& argument)
{
    Sequence& sequence = pool.current();

    if (argument.empty())
    {
        std::cout << "Tracks (current sequence):\n";
        printTracks(sequence);
        std::cout << "Usage: m <index>  |  m <index> on/off\n";
        return true;
    }

    std::istringstream stream(argument);
    std::size_t index = 0;
    stream >> index;

    if (stream.fail())
    {
        std::cout << "Invalid track index. Example: m 0\n";
        return true;
    }

    if (index >= sequence.trackCount())
    {
        std::cout << "Track index out of range (0-" << sequence.trackCount() - 1 << ")\n";
        return true;
    }

    std::string state;
    stream >> state;

    if (stream.fail())
    {
        const bool muted = !sequence.track(index).isMuted();
        sequence.setTrackMuted(index, muted);
        std::cout << "Track [" << index << "] " << sequence.track(index).name()
                  << (muted ? " muted\n" : " unmuted\n");
        return true;
    }

    if (state == "on" || state == "mute")
    {
        sequence.setTrackMuted(index, true);
        std::cout << "Track [" << index << "] " << sequence.track(index).name() << " muted\n";
    }
    else if (state == "off" || state == "unmute")
    {
        sequence.setTrackMuted(index, false);
        std::cout << "Track [" << index << "] " << sequence.track(index).name() << " unmuted\n";
    }
    else
    {
        std::cout << "Invalid mute state. Use on/off. Example: m 0 off\n";
    }

    return true;
}

void printPosition(const Sequence& sequence)
{
    const int tickIndex = sequence.position() == 0 ? 0 : sequence.position() - 1;
    const TransportPosition time = sequence.transportPosition(tickIndex);

    std::cout << "Position: bar " << time.bar << " / " << sequence.barCount()
              << "  beat " << time.beat << " / " << sequence.beatsPerBar()
              << "  tick " << time.tick << " / " << sequence.beatDuration()
              << "  (" << time.toString() << ")\n";
}

void maybePrintBarPosition(const Sequence& sequence)
{
    const int tickIndex = sequence.position();
    const int ticksPerBar = sequence.beatsPerBar() * sequence.beatDuration();

    if (tickIndex % ticksPerBar != 0)
        return;

    const TransportPosition time = sequence.transportPosition(tickIndex);
    std::cout << "--- Bar " << time.bar << " ---\n";
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
        SequencePool pool = SequencePool::createDefault(sender);

        clock.setBpm(kDefaultBpm);

        clock.setOnPlay([&pool]() {
            pool.resetCurrent();
        });

        clock.setOnTick([&pool](int tick) {
            maybePrintBarPosition(pool.current());
            pool.processTick();
        });

        clock.setOnStop([&pool]() {
            pool.allNotesOff();
        });

        std::cout << "Loaded " << pool.size() << " sequences\n";
        printPoolStatus(pool);

        std::cout << "Commands: [p]lay  [s]top  [n]ext  [b]ack  pos  [t]empo <bpm>  [m]ute <index>  [q]uit\n";

        while (gKeepRunning)
        {
            if (clock.isPlaying())
            {
                std::cout << "[seq " << pool.currentIndex() + 1 << " | "
                          << pool.current().formatPlayhead() << "] ";
            }

            std::cout << "> ";
            std::string command;
            if (!std::getline(std::cin, command))
                break;

            if (command == "p" || command == "play")
                clock.play();
            else if (command == "s" || command == "stop")
                clock.stop();
            else if (command == "n" || command == "next")
                pool.requestNext(!clock.isPlaying());
            else if (command == "b" || command == "back" || command == "previous" || command == "prev")
                pool.requestPrevious(!clock.isPlaying());
            else if (command == "pos" || command == "position")
                printPosition(pool.current());
            else if (command == "q" || command == "quit")
                break;
            else if (command == "t" || command == "tempo" || command == "bpm")
                trySetTempo(clock, "");
            else if (command.rfind("t ", 0) == 0 || command.rfind("tempo ", 0) == 0 || command.rfind("bpm ", 0) == 0)
            {
                const auto space = command.find(' ');
                trySetTempo(clock, command.substr(space + 1));
            }
            else if (command == "m" || command == "mute")
                tryMuteTrack(pool, "");
            else if (command.rfind("m ", 0) == 0 || command.rfind("mute ", 0) == 0)
            {
                const auto space = command.find(' ');
                tryMuteTrack(pool, command.substr(space + 1));
            }
            else if (!command.empty())
                std::cout << "Unknown command. Use p/s/n/b/pos/t/m/q.\n";
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
