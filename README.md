# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, box art, or game assets.

## Status

Player UI is a separate **Tauri + React** launcher (`gen3recomp-launcher`) with cover grid and HeroUI. The C++ host runs the game session after Build/Play.

**End users:** download a multi-OS player package from [GitHub Releases](../../releases) and follow [docs/player-guide.md](docs/player-guide.md).

OpenSpec: [openspec/README.md](openspec/README.md) · [docs/manual-boot.md](docs/manual-boot.md) · [docs/player-guide.md](docs/player-guide.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

## Download (Releases)

GitHub Actions builds **Linux**, **Windows**, and **macOS (arm64)** packages on tags matching `v*` and publishes them to Releases. Each zip includes the host, launcher, empty `roms/` folder, and docs — never ROMs/BIOS/covers/cart AOT.

```text
Unzip → put USA dumps in roms/ → put gba_bios.bin next to gen3recomp-player → run gen3recomp-player → Build → Play
```

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

Player package (host + launcher, D7-safe):

```sh
./scripts/package_release.sh
# → dist/release/gen3recomp-<ver>-linux-x64.zip
# Windows: powershell -File scripts/package_release.ps1
# macOS:   GEN3RECOMP_PLATFORM=macos-arm64 ./scripts/package_release.sh
```

| OS | Notes |
|----|--------|
| **Linux** | Reference path; cart AOT `abi3-linux-x64` / `libcart.so` |
| **Windows** | WebView2; host under `%APPDATA%/gen3recomp`; cart `abi3-windows-x64` / `cart.dll`. Cart Build needs Git Bash/WSL. Unsigned binaries may trigger SmartScreen. |
| **macOS** | CI ships **arm64**; data under Application Support; ABI `abi3-macos-arm64`. Gatekeeper may require right-click → Open. |

Cart **Build** shells to `scripts/build_cart_artifact.sh` (bash). On Windows use WSL/Git Bash for that script until a native script lands.

## Player flow

1. Place USA dumps in package/`roms/` (or `build/roms/` / repo `roms/` when developing) and `gba_bios.bin` beside the package root (or CWD).
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
| ROMs (player package) | `<package>/roms/*.gba` |
| ROMs (dev) | `build/roms/` or repo `roms/` |
| BIOS | `<package>/gba_bios.bin` or `./gba_bios.bin` |
| Saves (cartridge only) | `<user-data>/saves/<sha1>.sav` |
| Cart AOT | `<user-data>/cart_aot/<sha1>/<abi>/<lib>` |
| Covers | `<user-data>/covers/` or `roms/covers/` |
| Heal cache | `<user-data>/recomp_cache/<sha1>/` |

User-data defaults:

| OS | Path |
|----|------|
| Linux | `~/.local/share/gen3recomp` |
| Windows | `%APPDATA%\gen3recomp` |
| macOS | `~/Library/Application Support/gen3recomp` |

MVP has **no save states** — only in-game cartridge saves (see [docs/player-guide.md](docs/player-guide.md)).

**Release zip (D7):** host + launcher + empty `roms/` + scripts via packaging scripts / CI — no ROM/BIOS/covers/cart objects. Player entrypoint: `./gen3recomp-player`.

## License

**PolyForm Noncommercial License 1.0.0** — see [LICENSE](LICENSE).
