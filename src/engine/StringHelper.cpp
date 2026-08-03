#include "StringHelper.h"

#include <cstring>

namespace StringHelper
{
void copyName(char* dest, const char* src, std::size_t destSize)
{
    if (destSize == 0) {
        return;
    }

    if (!src) {
        dest[0] = '\0';
        return;
    }

    std::strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}
} // namespace StringHelper
