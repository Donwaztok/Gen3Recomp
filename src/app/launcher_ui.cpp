#include "launcher_ui.hpp"

#include "cart_artifact.hpp"
#include "cart_build.hpp"
#include "catalog.hpp"
#include "file_bytes.hpp"
#include "mods.hpp"
#include "roms_scan.hpp"

#include <SDL3/SDL.h>
#include "sdl3_dyn.hpp"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>
#include <vector>

namespace gen3recomp {
namespace {

struct LauncherState {
    Catalog catalog = Catalog::builtin();
    std::vector<DiscoveredRom> roms;
    std::vector<ModPackage> mods;
    int selected = 0;
    int mod_selected = 0;
    bool show_mods = false;
    std::optional<std::filesystem::path> bios_path;
    bool bios_ok = false;
    std::string status;
    std::string build_log_tail;
    std::atomic<bool> building{false};
    std::atomic<bool> build_done{false};
    std::atomic<bool> build_ok{false};
    std::string build_error;
    std::mutex log_mu;
    bool dialog_open = false;
    std::mutex pending_mu;
    std::string pending_add_error;
    std::optional<std::filesystem::path> pending_add_path;
};

void draw_line(SDL_Renderer* renderer, float x, float y, const std::string& text) {
    sdl3::RenderDebugText(renderer, x, y, text.c_str());
}

std::optional<std::filesystem::path> find_bios(const std::optional<std::filesystem::path>& preferred) {
    if (preferred.has_value() && std::filesystem::is_regular_file(*preferred)) {
        return preferred;
    }
    const std::filesystem::path local{"gba_bios.bin"};
    if (std::filesystem::is_regular_file(local)) {
        return local;
    }
    return std::nullopt;
}

bool bios_is_valid(const std::filesystem::path& path) {
    return is_known_bios_sha1(sha1_file(path));
}

void refresh_roms(LauncherState& state, const std::optional<std::filesystem::path>& exe_dir) {
    state.roms = scan_catalogued_roms(state.catalog, exe_dir);
    if (state.selected >= static_cast<int>(state.roms.size())) {
        state.selected = state.roms.empty() ? 0 : static_cast<int>(state.roms.size()) - 1;
    }
}

void refresh_mods(LauncherState& state) {
    state.mods = discover_mods();
    if (state.mod_selected >= static_cast<int>(state.mods.size())) {
        state.mod_selected = state.mods.empty() ? 0 : static_cast<int>(state.mods.size()) - 1;
    }
}

void refresh_bios(LauncherState& state, const std::optional<std::filesystem::path>& preferred) {
    state.bios_path = find_bios(preferred);
    state.bios_ok = state.bios_path.has_value() && bios_is_valid(*state.bios_path);
}

bool play_ready(const LauncherState& state) {
    if (!state.bios_ok || state.roms.empty()) {
        return false;
    }
    if (state.selected < 0 || state.selected >= static_cast<int>(state.roms.size())) {
        return false;
    }
    const auto& rom = state.roms[static_cast<size_t>(state.selected)];
    const auto kind = detect_cart_coverage(rom.sha1);
    return kind == CartCoverageKind::LinkedArtifact ||
           kind == CartCoverageKind::LinkedGeneratedCorpus ||
           kind == CartCoverageKind::RuntimeLoaded ||
           cart_artifact_ready(rom.sha1);
}

void start_build(LauncherState& state) {
    if (state.building || state.roms.empty()) {
        return;
    }
    const auto rom = state.roms[static_cast<size_t>(state.selected)];
    state.building = true;
    state.build_done = false;
    state.build_ok = false;
    state.build_error.clear();
    {
        std::lock_guard lock{state.log_mu};
        state.build_log_tail.clear();
        state.status = "Building cart AOT…";
    }
    std::thread([&state, rom]() {
        std::string error;
        const bool ok = build_cart_artifact(
            rom.path,
            [&state](const std::string& line) {
                std::lock_guard lock{state.log_mu};
                state.build_log_tail = line;
            },
            error);
        state.build_ok = ok;
        if (!ok) {
            state.build_error = error;
        }
        state.build_done = true;
        state.building = false;
    }).detach();
}

void on_file_dialog(
    void* userdata,
    const char* const* filelist,
    int filter) {
    (void)filter;
    auto* state = static_cast<LauncherState*>(userdata);
    std::lock_guard lock{state->pending_mu};
    state->dialog_open = false;
    if (filelist == nullptr) {
        state->pending_add_error = std::string("file dialog error: ") + sdl3::GetError();
        return;
    }
    if (filelist[0] == nullptr) {
        return;  // cancelled
    }
    state->pending_add_path = std::filesystem::path{filelist[0]};
}

}  // namespace

LauncherResult run_launcher_ui(
    const std::optional<std::filesystem::path>& preferred_bios,
    const std::optional<std::filesystem::path>& exe_dir) {
    LauncherResult result;
    {
        std::string load_error;
        if (!sdl3::load(load_error)) {
            result.code = ExitCode::InputError;
            result.message = "error: " + load_error + "\n";
            spdlog::error("{}", result.message);
            return result;
        }
        if (sdl3::SetHint != nullptr) {
            sdl3::SetHint("SDL_RENDER_DRIVER", "software");
            sdl3::SetHint("SDL_VIDEO_DRIVER", "x11");
        }
    }

    // Prefer a reliable present path on NVIDIA/Wayland (same as game session).
    if (std::getenv("SDL_RENDER_DRIVER") == nullptr) {
        setenv("SDL_RENDER_DRIVER", "software", 1);
    }

    if (!sdl3::Init(SDL_INIT_VIDEO)) {
        result.code = ExitCode::InputError;
        result.message = std::string("error: SDL init failed: ") + sdl3::GetError() + "\n";
        spdlog::error("{}", result.message);
        return result;
    }

    SDL_Window* window =
        sdl3::CreateWindow("gen3recomp - Launcher", 720, 480, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        result.code = ExitCode::InputError;
        result.message = std::string("error: failed to open launcher window: ") + sdl3::GetError() + "\n";
        spdlog::error("{}", result.message);
        sdl3::QuitSubSystem(SDL_INIT_VIDEO);
        return result;
    }
    SDL_Renderer* renderer = sdl3::CreateRenderer(window, "software");
    if (renderer == nullptr) {
        renderer = sdl3::CreateRenderer(window, nullptr);
    }
    if (renderer == nullptr) {
        result.code = ExitCode::InputError;
        result.message = std::string("error: failed to create renderer: ") + sdl3::GetError() + "\n";
        spdlog::error("{}", result.message);
        sdl3::DestroyWindow(window);
        sdl3::QuitSubSystem(SDL_INIT_VIDEO);
        return result;
    }

    LauncherState state;
    refresh_roms(state, exe_dir);
    refresh_mods(state);
    refresh_bios(state, preferred_bios);
    state.status = state.roms.empty()
                       ? "No catalogued ROMs in roms/. Press A to Add ROM…"
                       : "Select a title, Build if needed, then Play.";

    bool running = true;
    while (running) {
        if (state.build_done.exchange(false)) {
            std::lock_guard lock{state.log_mu};
            if (state.build_ok) {
                state.status = "Build complete — Play unlocked.";
            } else {
                state.status = "Build failed: " + state.build_error;
            }
        }
        if (state.pending_add_path.has_value() || !state.pending_add_error.empty()) {
            std::optional<std::filesystem::path> path;
            std::string add_error;
            {
                std::lock_guard lock{state.pending_mu};
                path = state.pending_add_path;
                state.pending_add_path.reset();
                add_error = std::move(state.pending_add_error);
                state.pending_add_error.clear();
            }
            if (!add_error.empty()) {
                state.status = add_error;
            } else if (path.has_value()) {
                const auto sha1 = sha1_file(*path);
                const auto game = state.catalog.find_by_sha1(sha1);
                if (!game.has_value()) {
                    state.status =
                        "Unsupported dump (SHA-1 " + sha1.substr(0, 12) +
                        "…) — MVP is USA Ruby/Sapphire/Emerald only.";
                } else {
                    std::error_code error;
                    std::filesystem::create_directories("roms", error);
                    const auto dest = std::filesystem::path{"roms"} / path->filename();
                    if (*path != dest) {
                        std::filesystem::copy_file(
                            *path,
                            dest,
                            std::filesystem::copy_options::overwrite_existing,
                            error);
                    }
                    refresh_roms(state, exe_dir);
                    for (int i = 0; i < static_cast<int>(state.roms.size()); ++i) {
                        if (state.roms[static_cast<size_t>(i)].sha1 == sha1) {
                            state.selected = i;
                            break;
                        }
                    }
                    state.status = "Added " + game->display_name;
                }
            }
        }

        SDL_Event event;
        while (sdl3::PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type != SDL_EVENT_KEY_DOWN) {
                continue;
            }
            const auto key = event.key.key;
            if (key == SDLK_ESCAPE || key == SDLK_Q) {
                running = false;
            } else if (state.show_mods) {
                if (key == SDLK_M || key == SDLK_BACKSPACE) {
                    state.show_mods = false;
                } else if (key == SDLK_UP && state.mod_selected > 0) {
                    --state.mod_selected;
                } else if (key == SDLK_DOWN &&
                           state.mod_selected + 1 < static_cast<int>(state.mods.size())) {
                    ++state.mod_selected;
                } else if ((key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_X) &&
                           !state.mods.empty()) {
                    auto& mod = state.mods[static_cast<size_t>(state.mod_selected)];
                    const bool next = !mod.enabled;
                    std::string error;
                    if (!set_mod_enabled(mod.id, next, error)) {
                        state.status = error;
                    } else {
                        state.status = std::string(next ? "Enabled " : "Disabled ") + mod.id;
                        refresh_mods(state);
                    }
                }
            } else {
                if (key == SDLK_UP && state.selected > 0) {
                    --state.selected;
                } else if (key == SDLK_DOWN &&
                           state.selected + 1 < static_cast<int>(state.roms.size())) {
                    ++state.selected;
                } else if (key == SDLK_A && !state.dialog_open) {
                    state.dialog_open = true;
                    static const SDL_DialogFileFilter filters[] = {
                        {"GBA ROM", "gba"},
                        {"All files", "*"},
                    };
                    sdl3::ShowOpenFileDialog(
                        on_file_dialog,
                        &state,
                        window,
                        filters,
                        2,
                        nullptr,
                        false);
                } else if (key == SDLK_B && !state.building && !state.roms.empty()) {
                    start_build(state);
                } else if (key == SDLK_M) {
                    refresh_mods(state);
                    state.show_mods = true;
                } else if (key == SDLK_R) {
                    refresh_roms(state, exe_dir);
                    refresh_bios(state, preferred_bios);
                    state.status = "Refreshed ROM list / BIOS.";
                } else if ((key == SDLK_RETURN || key == SDLK_X) && play_ready(state) &&
                           !state.building) {
                    result.play = true;
                    result.rom_path = state.roms[static_cast<size_t>(state.selected)].path;
                    result.bios_path = state.bios_path;
                    running = false;
                }
            }
        }

        sdl3::SetRenderDrawColor(renderer, 18, 22, 28, 255);
        sdl3::RenderClear(renderer);
        sdl3::SetRenderDrawColor(renderer, 220, 230, 240, 255);

        float y = 12.0f;
        draw_line(renderer, 12, y, "gen3recomp");
        y += 20;
        draw_line(renderer, 12, y, "Launcher — USA Ruby / Sapphire / Emerald");
        y += 24;

        const std::string bios_line =
            state.bios_ok ? ("BIOS: OK (" + state.bios_path->string() + ")")
                          : "BIOS: missing or unsupported (need ./gba_bios.bin)";
        draw_line(renderer, 12, y, bios_line.substr(0, 88));
        y += 20;

        if (state.show_mods) {
            draw_line(renderer, 12, y, "Mods (Enter toggle, M back)");
            y += 18;
            if (state.mods.empty()) {
                draw_line(renderer, 12, y, "  (no mods — place packages in mods/<id>/mod.toml)");
            }
            for (int i = 0; i < static_cast<int>(state.mods.size()); ++i) {
                const auto& mod = state.mods[static_cast<size_t>(i)];
                std::string line = (i == state.mod_selected ? "> " : "  ");
                line += mod.enabled ? "[x] " : "[ ] ";
                line += mod.name + " (" + mod.id + ")";
                if (!mod.valid) {
                    line += " INVALID";
                }
                draw_line(renderer, 12, y, line.substr(0, 90));
                y += 16;
                if (!mod.invalid_reason.empty() && i == state.mod_selected) {
                    draw_line(renderer, 24, y, mod.invalid_reason.substr(0, 88));
                    y += 16;
                }
            }
        } else {
            draw_line(renderer, 12, y, "ROMs in roms/  (Up/Down select)");
            y += 18;
            if (state.roms.empty()) {
                draw_line(renderer, 12, y, "  (none catalogued yet)");
                y += 16;
            }
            for (int i = 0; i < static_cast<int>(state.roms.size()); ++i) {
                const auto& rom = state.roms[static_cast<size_t>(i)];
                const bool ready = cart_artifact_ready(rom.sha1) ||
                                   detect_cart_coverage(rom.sha1) == CartCoverageKind::LinkedArtifact ||
                                   detect_cart_coverage(rom.sha1) ==
                                       CartCoverageKind::LinkedGeneratedCorpus;
                std::string line = (i == state.selected ? "> " : "  ");
                line += rom.game.display_name;
                line += ready ? "  [AOT ready]" : "  [needs Build]";
                draw_line(renderer, 12, y, line.substr(0, 90));
                y += 16;
            }
            y += 8;
            const bool can_play = play_ready(state);
            draw_line(
                renderer,
                12,
                y,
                can_play ? "Play: ENABLED (Enter / X)" : "Play: disabled (need BIOS + AOT)");
            y += 16;
            draw_line(
                renderer,
                12,
                y,
                state.building ? "Build: in progress…" : "Build: press B (needs c++ toolchain)");
            y += 20;
            draw_line(renderer, 12, y, "A Add ROM…   M Mods   R Refresh   Esc Quit");
        }

        y = 420.0f;
        {
            std::lock_guard lock{state.log_mu};
            draw_line(renderer, 12, y, ("Status: " + state.status).substr(0, 90));
            y += 16;
            if (!state.build_log_tail.empty()) {
                draw_line(renderer, 12, y, state.build_log_tail.substr(0, 90));
            }
        }

        sdl3::RenderPresent(renderer);
        sdl3::Delay(16);
    }

    sdl3::DestroyRenderer(renderer);
    sdl3::DestroyWindow(window);
    sdl3::QuitSubSystem(SDL_INIT_VIDEO);

    if (!result.play) {
        result.code = ExitCode::Ok;
        result.message.clear();
    }
    return result;
}

}  // namespace gen3recomp
