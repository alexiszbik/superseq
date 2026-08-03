#include "MidiClock.h"

#include <iostream>
#include <stdexcept>

namespace
{
constexpr double kMinBpm = 20.0;
constexpr double kMaxBpm = 300.0;
} // namespace

MidiClock::MidiClock(MidiInOut& midi)
    : midi_(midi)
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

void MidiClock::setOnPlay(TransportCallback callback)
{
    std::lock_guard lock(mutex_);
    onPlay_ = std::move(callback);
}

void MidiClock::setOnStop(TransportCallback callback)
{
    std::lock_guard lock(mutex_);
    onStop_ = std::move(callback);
}

void MidiClock::setOnTick(TickCallback callback)
{
    std::lock_guard lock(mutex_);
    onTick_ = std::move(callback);
}

void MidiClock::play()
{
    TransportCallback onPlay;
    {
        std::lock_guard lock(mutex_);
        if (playing_)
            return;

        currentTick_.store(0);
        onPlay = onPlay_;
    }

    if (onPlay)
        onPlay();

    midi_.sendStart();

    {
        std::lock_guard lock(mutex_);
        playing_.store(true);
    }
    cv_.notify_one();

    std::cout << "MIDI clock started at " << bpm() << " BPM\n";
}

void MidiClock::stop()
{
    TransportCallback onStop;
    {
        std::lock_guard lock(mutex_);
        if (!playing_)
            return;

        playing_.store(false);
        onStop = onStop_;
    }

    midi_.sendStop();

    if (onStop)
        onStop();

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
            const int tick = currentTick_.load();

            TickCallback onTick;
            {
                std::lock_guard lock(mutex_);
                onTick = onTick_;
            }

            if (onTick) {
                onTick(tick);
            }

            if (tick % 4 == 0) {
                midi_.sendClock();
            }

            currentTick_.store(tick + 1);

            nextTick += tickInterval();
            std::this_thread::sleep_until(nextTick);
        }
    }
}
