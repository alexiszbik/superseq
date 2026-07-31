#pragma once

#include "VirtualMidiSender.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class MidiClock
{
public:
    static constexpr int kPulsesPerQuarterNote = 24;

    explicit MidiClock(VirtualMidiSender& sender);
    ~MidiClock();

    MidiClock(const MidiClock&) = delete;
    MidiClock& operator=(const MidiClock&) = delete;

    void setBpm(double bpm);
    double bpm() const;

    void play();
    void stop();

    bool isPlaying() const noexcept { return playing_.load(); }

private:
    void run();
    std::chrono::nanoseconds tickInterval() const;

    VirtualMidiSender& sender_;

    mutable std::mutex mutex_;
    double bpm_ = 120.0;

    std::atomic<bool> playing_{ false };
    std::atomic<bool> shutdown_{ false };

    std::condition_variable cv_;
    std::thread thread_;
};
