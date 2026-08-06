# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, box art, or game assets.

## Status

Player UI is a separate **Tauri + React** launcher (`gen3recomp-launcher`) with cover grid and HeroUI. The C++ host runs the game session after Build/Play.

OpenSpec: [openspec/README.md](openspec/README.md) · [docs/manual-boot.md](docs/manual-boot.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

## Build host (Linux / Windows / macOS)

Dependencies: **C++20**, **CMake ≥ 3.20**, **SDL3**, **SDL2** (for gba-recomp).

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
```

## Build launcher (Tauri)

Dependencies: **Node 20+**, **Rust** (rustup), platform WebView (Linux: `webkit2gtk`).

```sh
cd launcher
npm install
npm run tauri:dev     # development (starts Vite on :1420)
npm run tauri:build   # production — embeds UI (required for Releases / ./scripts/run_launcher.sh)
```

**Do not** run bare `cargo build` inside `launcher/src-tauri` for a player binary: that often leaves the WebView pointed at `http://localhost:1420` and shows **Connection refused**. Use `npm run tauri:build` or `npm run tauri:dev`.

Player Release zip (host + launcher, D7-safe):

```sh
./scripts/package_release.sh
# → dist/release/gen3recomp-<ver>-linux-x64.zip
```

| OS | Notes |
|----|--------|
| **Linux** | Reference path; cart AOT `abi3-linux-x64` / `libcart.so` |
| **Windows** | WebView2; host under `%APPDATA%/gen3recomp`; cart `abi3-windows-x64` / `cart.dll` |
| **macOS** | Host data under Application Support; ABI `abi3-macos-arm64` or `abi3-macos-x64` |

Cart **Build** shells to `scripts/build_cart_artifact.sh` (bash). On Windows use WSL/Git Bash for that script until a native script lands.

## Player flow

1. Place USA dumps in `roms/` and `gba_bios.bin` in the working directory.
2. Optional cover overrides: `roms/covers/<game-id>.png` (gitignored). Otherwise covers fetch into user-data cache — never shipped in Releases.
3. Run the launcher (preferred):

```sh
cd launcher && npm run tauri:dev
# or after build (Hyprland/NVIDIA: prefer the wrapper — forces X11 for GTK/WebKit):
./scripts/run_launcher.sh
# or: GDK_BACKEND=x11 ./launcher/src-tauri/target/release/gen3recomp-launcher
```

On Wayland + NVIDIA, bare launch often fails with `Gdk-Message: Error 71` — use `GDK_BACKEND=x11` (the wrapper sets this).

Bare `./build/gen3recomp` **execs** `gen3recomp-launcher` if found beside the host / under `launcher/src-tauri/target/{debug,release}/`, else prints how to build it. Override with `GEN3RECOMP_LAUNCHER`. Legacy SDL UI: `GEN3RECOMP_SDL_LAUNCHER=1`.

Click a cover → **Build** (once) → **Play**.

## CLI

```sh
./build/gen3recomp --rom "./roms/Pokemon - Emerald Version (USA, Europe).gba" --bios ./gba_bios.bin
```

## Paths

| Kind | Location |
|------|----------|
| Saves | `<user-data>/saves/<sha1>.sav` |
| Cart AOT | `<user-data>/cart_aot/<sha1>/<abi>/<lib>` |
| Covers | `<user-data>/covers/` or `roms/covers/` |
| Heal cache | `<user-data>/recomp_cache/<sha1>/` |

Linux user-data default: `~/.local/share/gen3recomp`.

**Release zip (D7):** host + launcher + scripts via `./scripts/package_release.sh` — no ROM/BIOS/covers/cart objects. Player entrypoint in the zip: `./gen3recomp-player`.

## License

**PolyForm Noncommercial License 1.0.0** — see [LICENSE](LICENSE).
