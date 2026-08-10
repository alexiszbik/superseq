#include "MidiClock.h"
#include "TerminalLogger.h"
#include "VirtualMidiSender.h"
#include "SequencePool.h"
#include "TransportPosition.h"

#include <csignal>
#include <cstdio>
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

bool trySetTempo(MidiClock& clock, Logger& logger, const std::string& argument)
{
    char buffer[128];

    if (argument.empty()) {
        std::snprintf(buffer, sizeof(buffer), "Current tempo: %.0f BPM\n", clock.bpm());
        logger.info(buffer);
        return true;
    }

    std::istringstream stream(argument);
    double bpm = 0.0;
    stream >> bpm;

    if (stream.fail() || !stream.eof()) {
        logger.info("Invalid tempo. Example: t 128\n");
        return true;
    }

    try {
        clock.setBpm(bpm);
        std::snprintf(buffer, sizeof(buffer), "Tempo set to %.0f BPM\n", clock.bpm());
        logger.info(buffer);
    } catch (const std::exception& error) {
        std::snprintf(buffer, sizeof(buffer), "%s\n", error.what());
        logger.info(buffer);
    }

    return true;
}

void printTracks(const Sequence& sequence, Logger& logger)
{
    char buffer[128];

    for (std::size_t i = 0; i < sequence.trackCount(); ++i) {
        const SequenceTrack& track = sequence.track(i);
        std::snprintf(
            buffer,
            sizeof(buffer),
            "  [%zu] %s%s\n",
            i,
            track.name(),
            track.isMuted() ? " (muted)" : "");
        logger.info(buffer);
    }
}

void printPoolStatus(const SequencePool& pool, Logger& logger)
{
    char buffer[128];
    const Sequence& sequence = pool.current();

    std::snprintf(
        buffer,
        sizeof(buffer),
        "Sequence %zu / %zu — %s\n",
        pool.currentIndex() + 1,
        pool.size(),
        sequence.name());
    logger.info(buffer);
    printTracks(sequence, logger);
}

bool tryMuteTrack(SequencePool& pool, Logger& logger, const std::string& argument)
{
    Sequence& sequence = pool.current();
    char buffer[128];

    if (argument.empty()) {
        logger.info("Tracks (current sequence):\n");
        printTracks(sequence, logger);
        logger.info("Usage: m <index>  |  m <index> on/off\n");
        return true;
    }

    std::istringstream stream(argument);
    std::size_t index = 0;
    stream >> index;

    if (stream.fail()) {
        logger.info("Invalid track index. Example: m 0\n");
        return true;
    }

    if (index >= sequence.trackCount()) {
        std::snprintf(
            buffer,
            sizeof(buffer),
            "Track index out of range (0-%zu)\n",
            sequence.trackCount() - 1);
        logger.info(buffer);
        return true;
    }

    std::string state;
    stream >> state;

    if (stream.fail()) {
        const bool muted = !sequence.track(index).isMuted();
        sequence.setTrackMuted(index, muted);
        std::snprintf(
            buffer,
            sizeof(buffer),
            "Track [%zu] %s %s\n",
            index,
            sequence.track(index).name(),
            muted ? "muted" : "unmuted");
        logger.info(buffer);
        return true;
    }

    if (state == "on" || state == "mute") {
        sequence.setTrackMuted(index, true);
        std::snprintf(
            buffer,
            sizeof(buffer),
            "Track [%zu] %s muted\n",
            index,
            sequence.track(index).name());
        logger.info(buffer);
    } else if (state == "off" || state == "unmute") {
        sequence.setTrackMuted(index, false);
        std::snprintf(
            buffer,
            sizeof(buffer),
            "Track [%zu] %s unmuted\n",
            index,
            sequence.track(index).name());
        logger.info(buffer);
    } else {
        logger.info("Invalid mute state. Use on/off. Example: m 0 off\n");
    }

    return true;
}

std::string formatPlayhead(const Sequence& sequence)
{
    const tick_t tickIndex = sequence.position() == 0 ? 0 : sequence.position() - 1;
    return TransportPosition::fromTickIndex(
        tickIndex,
        sequence.beatsPerBar(),
        sequence.lengthInTicks()).toString();
}

void printPosition(const Sequence& sequence, Logger& logger)
{
    const tick_t tickIndex = sequence.position() == 0 ? 0 : sequence.position() - 1;
    const TransportPosition time = TransportPosition::fromTickIndex(
        tickIndex,
        sequence.beatsPerBar(),
        sequence.lengthInTicks());

    char buffer[128];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "Position: bar %d / %d  beat %d / %d  tick %d / %d  (%s)\n",
        time.bar,
        sequence.barCount(),
        time.beat,
        sequence.beatsPerBar(),
        time.tick,
        TransportPosition::kTicksPerQuarterNote,
        time.toString().c_str());
    logger.info(buffer);
}

void maybePrintBarPosition(const Sequence& sequence, Logger& logger)
{
    const tick_t tickIndex = sequence.position();
    const int ticksPerBar = sequence.beatsPerBar() * TransportPosition::kTicksPerQuarterNote;

    if (tickIndex % ticksPerBar != 0) {
        return;
    }

    const TransportPosition time = TransportPosition::fromTickIndex(
        tickIndex,
        sequence.beatsPerBar(),
        sequence.lengthInTicks());

    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "--- Bar %d ---\n", time.bar);
    logger.info(buffer);
}
} // namespace

int main()
{
    std::signal(SIGINT, handleSignal);

    constexpr auto kPortName = "ProtoSeq Virtual";
    constexpr double kDefaultBpm = 120.0;

    try
    {
        TerminalLogger logger;
        VirtualMidiSender sender(kPortName, logger);
        MidiClock clock(sender, logger);
        SequencePool pool = SequencePool::createDefault(sender, logger);

        clock.setBpm(kDefaultBpm);

        clock.setOnPlay([&pool]() {
            pool.resetCurrent();
        });

        clock.setOnTick([&pool, &logger](int) {
            maybePrintBarPosition(pool.current(), logger);
            pool.processTick();
        });

        clock.setOnStop([&pool]() {
            pool.allNotesOff();
        });

        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "Loaded %zu sequences\n", pool.size());
        logger.info(buffer);
        printPoolStatus(pool, logger);

        logger.info(
            "Commands: [p]lay  [s]top  [n]ext  [b]ack  pos  [t]empo <bpm>  [m]ute <index>  [q]uit\n");

        while (gKeepRunning)
        {
            if (clock.isPlaying()) {
                std::snprintf(
                    buffer,
                    sizeof(buffer),
                    "[seq %zu %s | %s] ",
                    pool.currentIndex() + 1,
                    pool.current().name(),
                    formatPlayhead(pool.current()).c_str());
                logger.info(buffer);
            }

            logger.info("> ");
            std::string command;
            if (!std::getline(std::cin, command)) {
                break;
            }

            if (command == "p" || command == "play") {
                clock.play();
            } else if (command == "s" || command == "stop") {
                clock.stop();
            } else if (command == "n" || command == "next") {
                pool.requestNext(!clock.isPlaying());
            } else if (command == "b" || command == "back" || command == "previous" || command == "prev") {
                pool.requestPrevious(!clock.isPlaying());
            } else if (command == "pos" || command == "position") {
                printPosition(pool.current(), logger);
            } else if (command == "q" || command == "quit") {
                break;
            } else if (command == "t" || command == "tempo" || command == "bpm") {
                trySetTempo(clock, logger, "");
            } else if (command.rfind("t ", 0) == 0 || command.rfind("tempo ", 0) == 0
                       || command.rfind("bpm ", 0) == 0) {
                const auto space = command.find(' ');
                trySetTempo(clock, logger, command.substr(space + 1));
            } else if (command == "m" || command == "mute") {
                tryMuteTrack(pool, logger, "");
            } else if (command.rfind("m ", 0) == 0 || command.rfind("mute ", 0) == 0) {
                const auto space = command.find(' ');
                tryMuteTrack(pool, logger, command.substr(space + 1));
            } else if (!command.empty()) {
                logger.info("Unknown command. Use p/s/n/b/pos/t/m/q.\n");
            }
        }

        clock.stop();
        logger.info("Bye.\n");
    }
    catch (const std::exception& error)
    {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Error: %s\n", error.what());
        TerminalLogger logger;
        logger.error(buffer);
        return 1;
    }

    return 0;
}
