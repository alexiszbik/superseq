#pragma once

#include <cstddef>

namespace StringHelper
{
constexpr std::size_t kNameMaxLength = 32;

using NameBuffer = char[kNameMaxLength];

void copyName(char* dest, const char* src, std::size_t destSize = kNameMaxLength);
} // namespace StringHelper
