#include <cstdint>

struct DispatchEntry {
    std::uint32_t addr;
    std::uint8_t thumb;
    std::uint8_t resume;
    void (*fn)(void);
};

extern "C" const DispatchEntry kDispatchTable[1] = {
    {0xFFFFFFFFu, 0u, 0u, nullptr},
};
extern "C" const unsigned kDispatchTableLen = 0u;
