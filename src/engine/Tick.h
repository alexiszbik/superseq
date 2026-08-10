#pragma once

#include <cstdint>
#include <limits>

using tick_t = uint16_t;

constexpr tick_t kMaxTick = 65535;

inline bool fitsInTickRange(uint32_t value) noexcept
{
    return value <= kMaxTick;
}
