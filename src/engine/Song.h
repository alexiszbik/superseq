#pragma once

#include "MidiInOut.h"
#include "Sequence.h"
#include "StringHelper.h"

#include <cstddef>
#include <vector>

class Song
{
public:
    explicit Song(const char* name);

    const char* name() const noexcept { return name_; }

    void add(Sequence sequence);
    std::size_t size() const noexcept { return sequences_.size(); }

    Sequence& sequence(std::size_t index);
    const Sequence& sequence(std::size_t index) const;

    void attachMidi(MidiInOut& midi);
    void allNotesOff();

private:
    StringHelper::NameBuffer name_ = {};
    std::vector<Sequence> sequences_;
};
