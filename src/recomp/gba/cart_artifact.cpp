#include "cart_artifact.hpp"

#include "user_data.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <mutex>
#include <string>

#if defined(GEN3RECOMP_HAS_GBARECOMP)
#include "cart_dispatch_host.hpp"
#endif

#if !defined(_WIN32)
#include <dlfcn.h>
#endif

namespace gen3recomp {
namespace {

struct LoadedCart {
    void* handle = nullptr;
    std::string sha1;
};

std::mutex g_cart_mu;
LoadedCart g_loaded_cart;

bool host_has_link_time_cart() {
#if defined(GEN3RECOMP_CART_VIA_ARTIFACT) || defined(GEN3RECOMP_HAS_STATIC_CART)
    return true;
#else
    return false;
#endif
}

}  // namespace

void clear_cart_artifact_activation() {
    std::lock_guard lock{g_cart_mu};
#if defined(GEN3RECOMP_HAS_GBARECOMP) && !defined(_WIN32)
    if (g_loaded_cart.handle != nullptr) {
        runtime_set_cart_dispatch(nullptr, 0);
        dlclose(g_loaded_cart.handle);
    }
#endif
    g_loaded_cart = {};
    clear_cart_artifact_runtime_loaded();
}

bool try_activate_cart_artifact(const std::string& rom_sha1, std::string& error) {
    if (host_has_link_time_cart()) {
        spdlog::info("cart dispatch already linked into host; skipping dlopen");
        return true;
    }

#if !defined(GEN3RECOMP_HAS_GBARECOMP)
    error = "gba-recomp runtime is not linked";
    return false;
#elif defined(_WIN32)
    error = "runtime cart dlopen is not implemented on Windows in this build";
    return false;
#else
    if (rom_sha1.empty()) {
        error = "ROM SHA-1 is empty";
        return false;
    }
    if (!cart_artifact_ready(rom_sha1)) {
        error = "cart artifact not found at " + cart_artifact_path(rom_sha1).string() +
                " — use Build in the launcher or scripts/build_cart_artifact.sh";
        return false;
    }

    std::lock_guard lock{g_cart_mu};
    if (g_loaded_cart.handle != nullptr && g_loaded_cart.sha1 == rom_sha1) {
        mark_cart_artifact_runtime_loaded(rom_sha1);
        return true;
    }

    if (g_loaded_cart.handle != nullptr) {
        runtime_set_cart_dispatch(nullptr, 0);
        dlclose(g_loaded_cart.handle);
        g_loaded_cart = {};
        clear_cart_artifact_runtime_loaded();
    }

    const auto path = cart_artifact_path(rom_sha1);
    const auto t0 = std::chrono::steady_clock::now();
    dlerror();
    // RTLD_LAZY: resolve undefined symbols on first use. RTLD_NOW paid a large
    // reloc cost on every Play for ~100MiB+ cart artifacts. We still dlsym the
    // dispatch table symbols immediately below.
    void* handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (handle == nullptr) {
        const char* detail = dlerror();
        error = std::string("failed to dlopen cart artifact: ") +
                (detail != nullptr ? detail : path.string());
        return false;
    }

    dlerror();
    void* table_sym = dlsym(handle, "kDispatchTable");
    void* len_sym = dlsym(handle, "kDispatchTableLen");
    const char* sym_err = dlerror();
    if (table_sym == nullptr || len_sym == nullptr) {
        error = std::string("cart artifact missing kDispatchTable / kDispatchTableLen") +
                (sym_err != nullptr ? std::string(": ") + sym_err : "");
        dlclose(handle);
        return false;
    }

    const unsigned len = *static_cast<const unsigned*>(len_sym);
    if (len == 0) {
        error = "cart artifact dispatch table length is zero";
        dlclose(handle);
        return false;
    }

    runtime_set_cart_dispatch(table_sym, len);
    g_loaded_cart.handle = handle;
    g_loaded_cart.sha1 = rom_sha1;
    mark_cart_artifact_runtime_loaded(rom_sha1);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - t0)
                        .count();
    spdlog::info(
        "activated cart artifact via dlopen entries={} path={} activate_ms={}",
        len,
        path.string(),
        ms);
    return true;
#endif
}

}  // namespace gen3recomp
