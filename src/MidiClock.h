#pragma once

#include "Logger.h"
#include "MidiInOut.h"

#include <functional>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class MidiClock
{
public:
    static constexpr int kPulsesPerQuarterNote = 96;

    using TickCallback = std::function<void(int tick)>;
    using TransportCallback = std::function<void()>;

    MidiClock(MidiInOut& midi, Logger& logger);
    ~MidiClock();

    MidiClock(const MidiClock&) = delete;
    MidiClock& operator=(const MidiClock&) = delete;

    void setBpm(double bpm);
    double bpm() const;

    void setOnPlay(TransportCallback callback);
    void setOnStop(TransportCallback callback);
    void setOnTick(TickCallback callback);

    void play();
    void stop();

    bool isPlaying() const noexcept { return playing_.load(); }
    int currentTick() const noexcept { return currentTick_.load(); }

private:
    void run();
    std::chrono::nanoseconds tickInterval() const;

    MidiInOut& midi_;
    Logger& logger_;

    mutable std::mutex mutex_;
    double bpm_ = 120.0;

    TransportCallback onPlay_;
    TransportCallback onStop_;
    TickCallback onTick_;

    std::atomic<bool> playing_{ false };
    std::atomic<bool> shutdown_{ false };
    std::atomic<int> currentTick_{ 0 };

    std::condition_variable cv_;
    std::thread thread_;
};
