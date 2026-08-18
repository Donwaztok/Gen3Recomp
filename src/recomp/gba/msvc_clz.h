#pragma once

// Forced include for gba-recomp bios_hle.cpp on MSVC only. Do not edit the
// submodule; __builtin_clz is a GCC/Clang builtin.
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
static __forceinline int gen3recomp_clz32(unsigned x) {
    if (x == 0) {
        return 32;
    }
    unsigned long index = 0;
    _BitScanReverse(&index, static_cast<unsigned long>(x));
    return 31 - static_cast<int>(index);
}
#define __builtin_clz(x) gen3recomp_clz32(x)
#endif
