#include "Song.h"

#include "StringHelper.h"

#include <utility>

Song::Song(const char* name)
{
    StringHelper::copyName(name_, name);
}

void Song::add(Sequence sequence)
{
    sequences_.push_back(std::move(sequence));
}

Sequence& Song::sequence(std::size_t index)
{
    return sequences_.at(index);
}

const Sequence& Song::sequence(std::size_t index) const
{
    return sequences_.at(index);
}

void Song::attachMidi(MidiInOut& midi)
{
    for (Sequence& sequence : sequences_) {
        sequence.attachMidi(midi);
    }
}

void Song::allNotesOff()
{
    for (Sequence& sequence : sequences_) {
        sequence.allNotesOff();
    }
}
