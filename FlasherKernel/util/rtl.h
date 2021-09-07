#pragma once

#include <cstdint>

#define offsetof __builtin_offsetof

typedef uint64_t size_t;

inline void memcpy(void* dst, const void* src, size_t count)
{
    uint8_t *d = (uint8_t*) dst, *s = (uint8_t*) src;
    for (size_t i = count; i > 0; i--) {
        *d++ = *s++;
    }
}

inline void memset(void* dst, uint8_t c, size_t count)
{
    uint8_t *d = (uint8_t*) dst;
    for (size_t i = count; i > 0; i--) {
        *d++ = c;
    }
}
