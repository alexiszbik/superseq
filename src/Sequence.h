#pragma once

#include "SequenceTrack.h"
#include "TransportPosition.h"
#include "VirtualMidiSender.h"

#include <cstddef>
#include <string>
#include <vector>

class Sequence
{
public:
    static constexpr int kTicksPerQuarterNote = 24;

    Sequence(
        int barCount,
        int beatsPerBar = 4,
        bool loop = true,
        int barLoop = 0,
        int beatDuration = kTicksPerQuarterNote);

    int barCount() const noexcept { return barCount_; }
    int beatsPerBar() const noexcept { return beatsPerBar_; }
    int beatDuration() const noexcept { return beatDuration_; }
    int loopInPoint() const noexcept { return loopInPoint_; }
    int barLoop() const noexcept { return loopInPoint_ / (beatsPerBar_ * beatDuration_); }
    bool isLooping() const noexcept { return looping_; }

    void setLooping(bool loop) noexcept { looping_ = loop; }

    int lengthInTicks() const noexcept;
    int position() const noexcept { return position_; }

    TransportPosition transportPosition(int tickIndex) const;
    std::string formatPlayhead() const;

    void addTrack(SequenceTrack track);
    void clearTracks();

    std::size_t trackCount() const noexcept { return tracks_.size(); }
    SequenceTrack& track(std::size_t index);
    const SequenceTrack& track(std::size_t index) const;

    void setTrackMuted(std::size_t index, bool muted, VirtualMidiSender& sender);

    void reset();
    void processTick(VirtualMidiSender& sender, int tick);
    void allNotesOff(VirtualMidiSender& sender);

    static Sequence createDemo(int barCount = 4);

private:
    static SequenceTrack createCMaj7Track(int lengthInTicks, int beatDuration);
    static SequenceTrack createKickSnareTrack(int lengthInTicks, int beatDuration);
    static SequenceTrack createKickSnareTrack2(int lengthInTicks, int beatDuration);
    static SequenceTrack bassLineTrack(int lengthInTicks, int beatDuration);

    int barCount_;
    int beatsPerBar_;
    bool looping_;
    int beatDuration_;

    int loopInPoint_ = 0;
    int position_ = 0;
    bool loopStartAfterWrap_ = false;
    std::vector<SequenceTrack> tracks_;
};
