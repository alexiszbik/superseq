#include "Sequence.h"

#include <algorithm>
#include <stdexcept>

Sequence::Sequence(int barCount, int beatsPerBar, bool loop, int beatDuration)
    : barCount_(barCount)
    , beatsPerBar_(beatsPerBar)
    , beatDuration_(beatDuration)
    , looping_(loop)
{
    if (barCount_ <= 0) {
        throw std::invalid_argument("Sequence bar count must be positive");
    }

    if (beatsPerBar_ <= 0) {
        throw std::invalid_argument("Sequence beats per bar must be positive");
    }
}

int Sequence::lengthInTicks() const noexcept
{
    return barCount_ * beatsPerBar_ * beatDuration_;
}

void Sequence::addNote(
    int startTick,
    int durationTicks,
    uint8_t note,
    uint8_t velocity,
    uint8_t channel)
{
    if (startTick < 0 || durationTicks <= 0)
        throw std::invalid_argument("Invalid note timing");

    addEvent({ startTick, channel, note, velocity, true });
    addEvent({ startTick + durationTicks, channel, note, 0, false });
}

void Sequence::addEvent(const Event& event)
{
    if (event.tick < 0 || event.tick >= lengthInTicks())
    {
        throw std::out_of_range(
            "Event tick " + std::to_string(event.tick) + " is outside sequence length "
            + std::to_string(lengthInTicks()));
    }

    events_.push_back(event);
    sortEvents();
}

void Sequence::sortEvents()
{
    std::sort(events_.begin(), events_.end(), [](const Event& a, const Event& b) {
        if (a.tick != b.tick)
            return a.tick < b.tick;
        return a.noteOn && !b.noteOn;
    });
}

void Sequence::reset()
{
    nextEventIndex_ = 0;
}

void Sequence::processTick(VirtualMidiSender& sender, int tick)
{
    const int length = lengthInTicks();
    if (length <= 0) {
        return;
    }

    if (!looping_ && tick >= length) {
        return;
    }

    const int position = looping_ ? (tick % length) : tick;

    if (position == 0 && tick > 0) {
        nextEventIndex_ = 0;
    }
 
    //Lookinf for next event
    while (nextEventIndex_ < events_.size() && events_[nextEventIndex_].tick < position) {
        ++nextEventIndex_;
    }

    while (nextEventIndex_ < events_.size() && events_[nextEventIndex_].tick == position)
    {
        const Event& event = events_[nextEventIndex_];

        if (event.noteOn) {
            sender.sendNoteOn(event.channel, event.note, event.velocity);
        }
        else {
            sender.sendNoteOff(event.channel, event.note, 0);
        }

        ++nextEventIndex_;
    }
}

void Sequence::allNotesOff(VirtualMidiSender& sender)
{
    for (uint8_t channel = 0; channel < 16; ++channel)
    {
        for (int note = 0; note < 128; ++note)
            sender.sendNoteOff(channel, static_cast<uint8_t>(note), 0);
    }
}

Sequence Sequence::createCMaj7Arpeggio(int barCount)
{
    Sequence sequence(barCount, 4, true);

    const uint8_t notes[] = { 60, 64, 67, 71 }; // C E G B
    const int beatCount = sequence.barCount() * sequence.beatsPerBar();
    const int noteDuration = kTicksPerQuarterNote - 2;

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * kTicksPerQuarterNote;
        sequence.addNote(tick, noteDuration, notes[beat % 4], 100);
    }

    return sequence;
}

Sequence Sequence::createKickSnarePattern(int barCount)
{
    Sequence sequence(barCount, 4, true);

    constexpr uint8_t kDrumChannel = 9; // MIDI channel 10
    constexpr uint8_t kKickNote = 36;
    constexpr uint8_t kSnareNote = 37;
    constexpr int kHitDuration = 10;

    const int beatCount = sequence.barCount() * sequence.beatsPerBar();

    for (int beat = 0; beat < beatCount; ++beat)
    {
        const int tick = beat * kTicksPerQuarterNote;
        const bool isKickBeat = (beat % 4) == 0 || (beat % 4) == 2;
        const uint8_t note = isKickBeat ? kKickNote : kSnareNote;

        sequence.addNote(tick, kHitDuration, note, 127, kDrumChannel);
    }

    return sequence;
}
