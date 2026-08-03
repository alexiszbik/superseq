#pragma once

#include "Sequence.h"

class SequenceFactory
{
public:
    static Sequence createSequenceOne(int barCount = 4);
    static Sequence createSequenceTwo(int barCount = 4);
    static Sequence createSequenceThree(int barCount = 4);
    static Sequence createSequenceFour(int barCount = 4);
    static Sequence createSequenceFive(int barCount = 4);
    static Sequence createSequenceSix(int barCount = 4);
    static Sequence createSequenceSeven(int barCount = 2);
};
