#pragma once

#include <cstdint>

// Host-side cart dispatch override. gba-recomp looks up kDispatchTable inside
// the submodule; we wrap overlay_try_dispatch / runtime_has_static_entry
// instead of patching that lookup.
extern "C" void runtime_set_cart_dispatch(const void* table, unsigned len);

namespace gen3recomp {

bool cart_dispatch_lookup(std::uint32_t pc, bool thumb, void (**fn)(), bool* resume);

}  // namespace gen3recomp
