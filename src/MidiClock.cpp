#include "MidiClock.h"

#include <iostream>
#include <stdexcept>

namespace
{
constexpr double kMinBpm = 20.0;
constexpr double kMaxBpm = 300.0;
} // namespace

MidiClock::MidiClock(VirtualMidiSender& sender)
    : sender_(sender)
{
    thread_ = std::thread(&MidiClock::run, this);
}

MidiClock::~MidiClock()
{
    shutdown_.store(true);
    playing_.store(false);
    cv_.notify_all();

    if (thread_.joinable())
        thread_.join();
}

void MidiClock::setBpm(double bpm)
{
    if (bpm < kMinBpm || bpm > kMaxBpm)
    {
        throw std::out_of_range(
            "BPM must be between " + std::to_string(static_cast<int>(kMinBpm))
            + " and " + std::to_string(static_cast<int>(kMaxBpm)));
    }

    std::lock_guard lock(mutex_);
    bpm_ = bpm;
}

double MidiClock::bpm() const
{
    std::lock_guard lock(mutex_);
    return bpm_;
}

void MidiClock::play()
{
    if (playing_.exchange(true))
        return;

    sender_.sendStart();
    cv_.notify_one();

    std::cout << "MIDI clock started at " << bpm() << " BPM\n";
}

void MidiClock::stop()
{
    if (!playing_.exchange(false))
        return;

    sender_.sendStop();
    cv_.notify_one();

    std::cout << "MIDI clock stopped\n";
}

std::chrono::nanoseconds MidiClock::tickInterval() const
{
    std::lock_guard lock(mutex_);
    const double ticksPerSecond = (bpm_ / 60.0) * static_cast<double>(kPulsesPerQuarterNote);
    const double secondsPerTick = 1.0 / ticksPerSecond;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(secondsPerTick));
}

void MidiClock::run()
{
    while (!shutdown_.load())
    {
        {
            std::unique_lock lock(mutex_);
            cv_.wait(lock, [this] { return playing_.load() || shutdown_.load(); });
        }

        if (shutdown_.load())
            break;

        auto nextTick = std::chrono::steady_clock::now();

        while (playing_.load())
        {
            sender_.sendClock();

            nextTick += tickInterval();
            std::this_thread::sleep_until(nextTick);
        }
    }
}
