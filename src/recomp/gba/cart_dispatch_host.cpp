#include "cart_dispatch_host.hpp"

#include <cstdint>

#if defined(GEN3RECOMP_WRAP_CART_DISPATCH)
#include "runtime_arm.h"
#endif

namespace {

struct DispatchEntry {
    std::uint32_t addr;
    std::uint8_t thumb;
    std::uint8_t resume;
    void (*fn)();
};

const DispatchEntry* g_cart_table = nullptr;
unsigned g_cart_len = 0;

const DispatchEntry* lookup_in(const DispatchEntry* table, unsigned len,
                               std::uint32_t pc, bool thumb) {
    unsigned lo = 0;
    unsigned hi = len;
    while (lo < hi) {
        const unsigned mid = (lo + hi) >> 1u;
        if (table[mid].addr < pc) {
            lo = mid + 1u;
        } else {
            hi = mid;
        }
    }
    for (unsigned i = lo; i < len && table[i].addr == pc; ++i) {
        if ((table[i].thumb != 0) == thumb) {
            return &table[i];
        }
    }
    return nullptr;
}

}  // namespace

extern "C" void runtime_set_cart_dispatch(const void* table, unsigned len) {
    if (table == nullptr || len == 0) {
        g_cart_table = nullptr;
        g_cart_len = 0;
        return;
    }
    g_cart_table = static_cast<const DispatchEntry*>(table);
    g_cart_len = len;
}

namespace gen3recomp {

bool cart_dispatch_lookup(std::uint32_t pc, bool thumb, void (**fn)(), bool* resume) {
    if (g_cart_table == nullptr || g_cart_len == 0) {
        return false;
    }
    const DispatchEntry* entry =
        lookup_in(g_cart_table, g_cart_len, pc & ~1u, thumb);
    if (entry == nullptr || entry->fn == nullptr) {
        return false;
    }
    if (fn != nullptr) {
        *fn = entry->fn;
    }
    if (resume != nullptr) {
        *resume = entry->resume != 0;
    }
    return true;
}

}  // namespace gen3recomp

#if defined(GEN3RECOMP_WRAP_CART_DISPATCH)

extern "C" int __real_overlay_try_dispatch(std::uint32_t pc, int thumb);
extern "C" int __real_runtime_has_static_entry(std::uint32_t pc, int thumb);

extern "C" int __wrap_overlay_try_dispatch(std::uint32_t pc, int thumb) {
    void (*fn)() = nullptr;
    bool resume = false;
    if (gen3recomp::cart_dispatch_lookup(pc, thumb != 0, &fn, &resume)) {
        g_runtime_resume_pc = resume ? (pc & ~1u) : 0u;
        fn();
        return 1;
    }
    return __real_overlay_try_dispatch(pc, thumb);
}

extern "C" int __wrap_runtime_has_static_entry(std::uint32_t pc, int thumb) {
    if (gen3recomp::cart_dispatch_lookup(pc, thumb != 0, nullptr, nullptr)) {
        return 1;
    }
    return __real_runtime_has_static_entry(pc, thumb);
}

#endif
