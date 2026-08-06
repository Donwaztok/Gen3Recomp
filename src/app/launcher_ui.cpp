#include "launcher_ui.hpp"

#include "cart_artifact.hpp"
#include "cart_build.hpp"
#include "catalog.hpp"
#include "cover_art.hpp"
#include "file_bytes.hpp"
#include "mods.hpp"
#include "roms_scan.hpp"
#include "sdl3_dyn.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace gen3recomp {
namespace {

constexpr int kWinW = 960;
constexpr int kWinH = 640;
constexpr float kTileW = 168.0f;
constexpr float kTileH = 220.0f;
constexpr float kGap = 24.0f;

struct TileCover {
    SDL_Texture* texture = nullptr;
    std::atomic<bool> loading{false};
    std::atomic<bool> failed{false};
};

struct UiButton {
    SDL_FRect rect{};
    const char* label = "";
    bool enabled = true;
};

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
    std::unordered_map<std::string, TileCover> covers;
    std::mutex cover_mu;
    float mouse_x = 0;
    float mouse_y = 0;
    bool mouse_down = false;
    bool mouse_clicked = false;
};

void draw_text(SDL_Renderer* r, float x, float y, const std::string& text) {
    sdl3::RenderDebugText(r, x, y, text.c_str());
}

void fill_rect(SDL_Renderer* r, const SDL_FRect& rect, Uint8 cr, Uint8 cg, Uint8 cb, Uint8 ca = 255) {
    sdl3::SetRenderDrawColor(r, cr, cg, cb, ca);
    sdl3::RenderFillRect(r, &rect);
}

void stroke_rect(SDL_Renderer* r, const SDL_FRect& rect, Uint8 cr, Uint8 cg, Uint8 cb) {
    sdl3::SetRenderDrawColor(r, cr, cg, cb, 255);
    sdl3::RenderRect(r, &rect);
}

bool point_in(const SDL_FRect& rect, float x, float y) {
    return x >= rect.x && y >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
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

bool aot_ready(const DiscoveredRom& rom) {
    const auto kind = detect_cart_coverage(rom.sha1);
    return kind == CartCoverageKind::LinkedArtifact ||
           kind == CartCoverageKind::LinkedGeneratedCorpus ||
           kind == CartCoverageKind::RuntimeLoaded ||
           cart_artifact_ready(rom.sha1);
}

bool play_ready(const LauncherState& state) {
    if (!state.bios_ok || state.roms.empty()) {
        return false;
    }
    if (state.selected < 0 || state.selected >= static_cast<int>(state.roms.size())) {
        return false;
    }
    return aot_ready(state.roms[static_cast<size_t>(state.selected)]);
}

SDL_Texture* texture_from_pixels(SDL_Renderer* renderer, const CoverPixels& pixels) {
    SDL_Texture* tex = sdl3::CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        pixels.width,
        pixels.height);
    if (tex == nullptr) {
        return nullptr;
    }
    sdl3::SetTextureScaleMode(tex, SDL_SCALEMODE_LINEAR);
    sdl3::UpdateTexture(
        tex,
        nullptr,
        pixels.rgba.data(),
        pixels.width * 4);
    return tex;
}

void ensure_cover_loading(LauncherState& state, SDL_Renderer* renderer, const std::string& game_id) {
    {
        std::lock_guard lock{state.cover_mu};
        auto& slot = state.covers[game_id];
        if (slot.texture != nullptr || slot.loading || slot.failed) {
            return;
        }
        if (auto path = find_cached_cover(game_id); path.has_value()) {
            if (auto pixels = load_cover_pixels(*path); pixels.has_value()) {
                slot.texture = texture_from_pixels(renderer, *pixels);
                if (slot.texture != nullptr) {
                    return;
                }
            }
        }
        slot.loading = true;
    }

    std::thread([&state, renderer, game_id]() {
        std::string error;
        auto path = fetch_cover_to_cache(game_id, error);
        std::lock_guard lock{state.cover_mu};
        auto& slot = state.covers[game_id];
        slot.loading = false;
        if (!path.has_value()) {
            slot.failed = true;
            spdlog::warn("cover fetch failed for {}: {}", game_id, error);
            return;
        }
        if (auto pixels = load_cover_pixels(*path); pixels.has_value()) {
            // Texture must be created on the render thread — stash path for main loop.
            // Mark failed false and leave texture null; main loop will load from cache.
            slot.failed = false;
        } else {
            slot.failed = true;
        }
        (void)renderer;
    }).detach();
}

void pump_cover_textures(LauncherState& state, SDL_Renderer* renderer) {
    std::lock_guard lock{state.cover_mu};
    for (auto& [id, slot] : state.covers) {
        if (slot.texture != nullptr || slot.loading || slot.failed) {
            continue;
        }
        if (auto path = find_cached_cover(id); path.has_value()) {
            if (auto pixels = load_cover_pixels(*path); pixels.has_value()) {
                slot.texture = texture_from_pixels(renderer, *pixels);
            } else {
                slot.failed = true;
            }
        }
    }
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

void on_file_dialog(void* userdata, const char* const* filelist, int filter) {
    (void)filter;
    auto* state = static_cast<LauncherState*>(userdata);
    std::lock_guard lock{state->pending_mu};
    state->dialog_open = false;
    if (filelist == nullptr) {
        state->pending_add_error = std::string("file dialog error: ") + sdl3::GetError();
        return;
    }
    if (filelist[0] == nullptr) {
        return;
    }
    state->pending_add_path = std::filesystem::path{filelist[0]};
}

SDL_FRect tile_rect(int index, int cols) {
    const int row = index / cols;
    const int col = index % cols;
    const float grid_w = cols * kTileW + (cols - 1) * kGap;
    const float origin_x = (static_cast<float>(kWinW) - grid_w) * 0.5f;
    const float origin_y = 96.0f;
    return SDL_FRect{
        origin_x + col * (kTileW + kGap),
        origin_y + row * (kTileH + kGap + 28.0f),
        kTileW,
        kTileH,
    };
}

UiButton make_button(float x, float y, float w, float h, const char* label, bool enabled) {
    return UiButton{SDL_FRect{x, y, w, h}, label, enabled};
}

void draw_button(SDL_Renderer* r, const UiButton& btn, bool hover) {
    const Uint8 shade = btn.enabled ? (hover ? 70 : 48) : 28;
    fill_rect(r, btn.rect, shade, shade + 8, shade + 16);
    stroke_rect(r, btn.rect, btn.enabled ? 180 : 90, btn.enabled ? 200 : 100, btn.enabled ? 220 : 110);
    draw_text(r, btn.rect.x + 14, btn.rect.y + btn.rect.h * 0.5f - 4, btn.label);
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
        }
    }
    if (std::getenv("SDL_RENDER_DRIVER") == nullptr) {
#if defined(_WIN32)
        _putenv_s("SDL_RENDER_DRIVER", "software");
#else
        setenv("SDL_RENDER_DRIVER", "software", 1);
#endif
    }

    if (!sdl3::Init(SDL_INIT_VIDEO)) {
        result.code = ExitCode::InputError;
        result.message = std::string("error: SDL init failed: ") + sdl3::GetError() + "\n";
        spdlog::error("{}", result.message);
        return result;
    }

    // Floating compact window (not maximized).
    SDL_Window* window = sdl3::CreateWindow("gen3recomp", kWinW, kWinH, SDL_WINDOW_RESIZABLE);
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
                       ? "Drop catalogued dumps in roms/ or click Add ROM"
                       : "Select a cover, then Build or Play";

    // Apply note (2.6): custom textured UI chosen over ImGui to avoid SDL2/SDL3 fights.
    spdlog::info("launcher UI: custom SDL3 textured grid (ImGui skipped)");

    bool running = true;
    while (running) {
        state.mouse_clicked = false;
        if (state.build_done.exchange(false)) {
            std::lock_guard lock{state.log_mu};
            state.status = state.build_ok ? "Build complete — Play unlocked" : ("Build failed: " + state.build_error);
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
                    state.status = "Unsupported dump — MVP is USA Ruby/Sapphire/Emerald only";
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
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                state.mouse_clicked = true;
                state.mouse_x = event.button.x;
                state.mouse_y = event.button.y;
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                state.mouse_x = event.motion.x;
                state.mouse_y = event.motion.y;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
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
                    } else if (key == SDLK_LEFT && state.selected > 0) {
                        --state.selected;
                    } else if (key == SDLK_RIGHT &&
                               state.selected + 1 < static_cast<int>(state.roms.size())) {
                        ++state.selected;
                    } else if (key == SDLK_A && !state.dialog_open) {
                        state.dialog_open = true;
                        static const SDL_DialogFileFilter filters[] = {
                            {"GBA ROM", "gba"},
                            {"All files", "*"},
                        };
                        sdl3::ShowOpenFileDialog(
                            on_file_dialog, &state, window, filters, 2, nullptr, false);
                    } else if (key == SDLK_B && !state.building && !state.roms.empty()) {
                        start_build(state);
                    } else if (key == SDLK_M) {
                        refresh_mods(state);
                        state.show_mods = true;
                    } else if (key == SDLK_R) {
                        refresh_roms(state, exe_dir);
                        refresh_bios(state, preferred_bios);
                        state.status = "Refreshed";
                    } else if ((key == SDLK_RETURN || key == SDLK_X) && play_ready(state) &&
                               !state.building) {
                        result.play = true;
                        result.rom_path = state.roms[static_cast<size_t>(state.selected)].path;
                        result.bios_path = state.bios_path;
                        running = false;
                    }
                }
            }
        }

        for (const auto& rom : state.roms) {
            ensure_cover_loading(state, renderer, rom.game.id);
        }
        pump_cover_textures(state, renderer);

        const int cols = std::max(1, static_cast<int>((kWinW - 48) / (kTileW + kGap)));
        const bool can_play = play_ready(state);
        auto btn_play = make_button(40, 560, 140, 40, "Play", can_play && !state.building);
        auto btn_build = make_button(200, 560, 140, 40, "Build", !state.roms.empty() && !state.building);
        auto btn_add = make_button(360, 560, 140, 40, "Add ROM", !state.dialog_open);
        auto btn_mods = make_button(520, 560, 140, 40, "Mods", true);
        auto btn_refresh = make_button(680, 560, 120, 40, "Refresh", true);

        if (state.mouse_clicked) {
            if (!state.show_mods) {
                for (int i = 0; i < static_cast<int>(state.roms.size()); ++i) {
                    if (point_in(tile_rect(i, cols), state.mouse_x, state.mouse_y)) {
                        state.selected = i;
                    }
                }
                if (point_in(btn_play.rect, state.mouse_x, state.mouse_y) && btn_play.enabled) {
                    result.play = true;
                    result.rom_path = state.roms[static_cast<size_t>(state.selected)].path;
                    result.bios_path = state.bios_path;
                    running = false;
                } else if (point_in(btn_build.rect, state.mouse_x, state.mouse_y) && btn_build.enabled) {
                    start_build(state);
                } else if (point_in(btn_add.rect, state.mouse_x, state.mouse_y) && btn_add.enabled) {
                    state.dialog_open = true;
                    static const SDL_DialogFileFilter filters[] = {
                        {"GBA ROM", "gba"},
                        {"All files", "*"},
                    };
                    sdl3::ShowOpenFileDialog(
                        on_file_dialog, &state, window, filters, 2, nullptr, false);
                } else if (point_in(btn_mods.rect, state.mouse_x, state.mouse_y)) {
                    refresh_mods(state);
                    state.show_mods = true;
                } else if (point_in(btn_refresh.rect, state.mouse_x, state.mouse_y)) {
                    refresh_roms(state, exe_dir);
                    refresh_bios(state, preferred_bios);
                    state.status = "Refreshed";
                }
            } else {
                for (int i = 0; i < static_cast<int>(state.mods.size()); ++i) {
                    SDL_FRect row{48, 120 + i * 36.0f, 700, 32};
                    if (point_in(row, state.mouse_x, state.mouse_y)) {
                        state.mod_selected = i;
                        auto& mod = state.mods[static_cast<size_t>(i)];
                        const bool next = !mod.enabled;
                        std::string error;
                        if (!set_mod_enabled(mod.id, next, error)) {
                            state.status = error;
                        } else {
                            state.status = std::string(next ? "Enabled " : "Disabled ") + mod.id;
                            refresh_mods(state);
                        }
                    }
                }
                SDL_FRect back{40, 560, 140, 40};
                if (point_in(back, state.mouse_x, state.mouse_y)) {
                    state.show_mods = false;
                }
            }
        }

        fill_rect(renderer, SDL_FRect{0, 0, static_cast<float>(kWinW), static_cast<float>(kWinH)}, 14, 16, 22);
        sdl3::SetRenderDrawColor(renderer, 230, 236, 245, 255);
        draw_text(renderer, 24, 20, "gen3recomp");
        draw_text(renderer, 24, 40, "Floating launcher — USA Ruby / Sapphire / Emerald");
        const std::string bios_line =
            state.bios_ok ? ("BIOS OK — " + state.bios_path->string()) : "BIOS missing (need ./gba_bios.bin)";
        draw_text(renderer, 24, 64, bios_line.substr(0, 100));

        if (state.show_mods) {
            draw_text(renderer, 24, 96, "Mods — click to toggle, Back to return");
            for (int i = 0; i < static_cast<int>(state.mods.size()); ++i) {
                const auto& mod = state.mods[static_cast<size_t>(i)];
                SDL_FRect row{48, 120 + i * 36.0f, 700, 32};
                const bool sel = i == state.mod_selected;
                fill_rect(renderer, row, sel ? 40 : 28, sel ? 48 : 32, sel ? 64 : 40);
                std::string line = (mod.enabled ? "[x] " : "[ ] ") + mod.name + " (" + mod.id + ")";
                if (!mod.valid) {
                    line += " INVALID";
                }
                draw_text(renderer, row.x + 10, row.y + 10, line.substr(0, 80));
            }
            if (state.mods.empty()) {
                draw_text(renderer, 48, 120, "No mods — place packages in mods/<id>/mod.toml");
            }
            draw_button(
                renderer,
                make_button(40, 560, 140, 40, "Back", true),
                point_in(SDL_FRect{40, 560, 140, 40}, state.mouse_x, state.mouse_y));
        } else {
            for (int i = 0; i < static_cast<int>(state.roms.size()); ++i) {
                const auto& rom = state.roms[static_cast<size_t>(i)];
                const SDL_FRect tile = tile_rect(i, cols);
                const bool sel = i == state.selected;
                const bool hover = point_in(tile, state.mouse_x, state.mouse_y);
                fill_rect(renderer, tile, 30, 34, 44);
                stroke_rect(
                    renderer,
                    tile,
                    sel ? 120 : (hover ? 90 : 55),
                    sel ? 200 : (hover ? 160 : 70),
                    sel ? 255 : (hover ? 200 : 90));

                SDL_Texture* tex = nullptr;
                {
                    std::lock_guard lock{state.cover_mu};
                    auto it = state.covers.find(rom.game.id);
                    if (it != state.covers.end()) {
                        tex = it->second.texture;
                    }
                }
                if (tex != nullptr) {
                    SDL_FRect inner{tile.x + 8, tile.y + 8, tile.w - 16, tile.h - 40};
                    sdl3::RenderTexture(renderer, tex, nullptr, &inner);
                } else {
                    fill_rect(
                        renderer,
                        SDL_FRect{tile.x + 8, tile.y + 8, tile.w - 16, tile.h - 40},
                        50,
                        40,
                        70);
                    draw_text(renderer, tile.x + 20, tile.y + tile.h * 0.4f, "No cover");
                }
                const bool ready = aot_ready(rom);
                draw_text(
                    renderer,
                    tile.x + 8,
                    tile.y + tile.h - 24,
                    (ready ? "AOT ready  " : "Needs Build  ") + rom.game.display_name);
            }
            if (state.roms.empty()) {
                draw_text(renderer, 48, 160, "No catalogued ROMs in roms/ yet");
            }

            draw_button(renderer, btn_play, point_in(btn_play.rect, state.mouse_x, state.mouse_y));
            draw_button(renderer, btn_build, point_in(btn_build.rect, state.mouse_x, state.mouse_y));
            draw_button(renderer, btn_add, point_in(btn_add.rect, state.mouse_x, state.mouse_y));
            draw_button(renderer, btn_mods, point_in(btn_mods.rect, state.mouse_x, state.mouse_y));
            draw_button(renderer, btn_refresh, point_in(btn_refresh.rect, state.mouse_x, state.mouse_y));
        }

        {
            std::lock_guard lock{state.log_mu};
            draw_text(renderer, 24, 610, ("Status: " + state.status).substr(0, 110));
            if (!state.build_log_tail.empty()) {
                draw_text(renderer, 24, 625, state.build_log_tail.substr(0, 110));
            }
        }
        draw_text(renderer, 700, 20, "Covers cached locally (D7)");

        sdl3::RenderPresent(renderer);
        sdl3::Delay(16);
    }

    {
        std::lock_guard lock{state.cover_mu};
        for (auto& [_, slot] : state.covers) {
            if (slot.texture != nullptr) {
                sdl3::DestroyTexture(slot.texture);
            }
        }
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
