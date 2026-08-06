# Manual boot checklist

MVP acceptance is title-screen boot for USA Ruby, Sapphire, and Emerald at playable speed when static cart AOT is present.

## Prerequisites

- USA dump(s) under package/`roms/`, `build/roms/`, or repo `roms/` — `*.gba` is gitignored
- GBA BIOS (`gba_bios.bin` beside the package/host or `--bios`)
- `git submodule update --init --recursive`
- SDL2 + SDL3
- Host C++ toolchain (`c++` on PATH) for the one-time cart Build

## Build host

```sh
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
./scripts/recompile_user_bios.sh /path/to/gba_bios.bin
ctest --test-dir build --output-on-failure
```

## Player path (launcher)

```sh
./build/gen3recomp
```

1. Confirm BIOS status is OK
2. Select Emerald (or R/S) from the `roms/` list — do not expect silent autoplay
3. Press **B** Build once if AOT is missing (progress in status line; can take several minutes)
4. Press **Enter** Play when unlocked — this must **not** re-run cart Build
5. Expect log: `activated cart artifact via dlopen` (`activate_ms=…`) and
   `self_heal_recompile=ENABLED … eager_warm=0 warm_ms=…`
6. With a populated heal cache, repeated Play should reach early boot in seconds.
   An empty heal cache may hitch on the first session while overlays are compiled;
   later Plays reuse those shards on demand (no full-cache warm at startup).

Diagnostic: `GBARECOMP_HEAL_EAGER_WARM=1` restores the old “load every overlay DLL
before the guest runs” behavior (slow; for measurement only).

## CLI path

```sh
./scripts/build_cart_artifact.sh /path/to/game.gba   # optional if built from UI
./build/gen3recomp --rom /path/to/game.gba --bios /path/to/gba_bios.bin
```

Stock hosts dlopen the user-data `libcart.so`. Optional contributor link: `-DGEN3RECOMP_CART_ARTIFACT=…`.

Configs: `data/emerald_usa.toml`, `data/ruby_usa.toml`, `data/sapphire_usa.toml` (auto-selected by ROM SHA-1).

**Dev link mode:** `./scripts/recompile_user_rom.sh` → `generated/rom/` → reconfigure without cart artifact (multi-minute compile into the exe).

`--prepare` is optional/diagnostic only (self-heal warm-up). Do not use it as a substitute for cart AOT.

## Manual checklist (tauri-react-launcher)

- [ ] `cd launcher && npm run tauri:dev` (or release binary) opens HeroUI cover library
- [ ] Covers appear after online fetch (or placeholders offline)
- [ ] Build then Play Emerald via launcher on Linux
- [ ] Unknown ROM rejected in Add ROM; CLI `--rom` still works without Tauri
- [ ] Bare `./build/gen3recomp` execs launcher when binary is present
- [ ] Win/macOS: launcher + host recipes followed from README

## Manual checklist (launcher-gui-covers — superseded UI)

- [x] Superseded by Tauri launcher as default player UI (`GEN3RECOMP_SDL_LAUNCHER=1` for legacy)


## Paths

| Kind | Location |
|------|----------|
| Cartridge save | `<user-data>/saves/<sha1>.sav` (Linux default `~/.local/share/gen3recomp`; Windows `%APPDATA%\gen3recomp`; macOS `~/Library/Application Support/gen3recomp`) |
| Cart AOT `.so` | `<user-data>/cart_aot/<sha1>/abi3-linux-x64/libcart.so` |
| IWRAM / heal DLLs | `<user-data>/recomp_cache/<sha1>/` (loaded on demand by default; not all shards at Play start) |
| Mod enablement | `<user-data>/mods_enabled.txt` |
| Eager heal warm (diag) | `GBARECOMP_HEAL_EAGER_WARM=1` — optional; slows every Play |

End-user download/play/save docs: [player-guide.md](player-guide.md). MVP has no save states.

## Controls

### Launcher

| Key | Action |
|-----|--------|
| Up/Down | Select ROM or mod |
| A | Add ROM… |
| B | Build cart AOT |
| Enter / X | Play (when ready) |
| M | Mods panel |
| R | Refresh |
| Esc / Q | Quit |

### In-game

| Key | GBA |
|-----|-----|
| Arrows | D-pad |
| X | A |
| Z | B |
| Enter | Start |
| Right Shift | Select |
| C | L |
| V | R |
| Esc or Q | Quit |

## Results template

```
Date:
Host:
Binary: gen3recomp
gba-recomp pin: (see third_party/gbarecomp)
Cart coverage: runtime-loaded-artifact / linked-artifact / linked-generated-rom / heal-only

Emerald USA
- SHA-1: f3ae088181bf583e55daf962a92bb46f4f1d07b7
- Launcher Build/Play: yes/no
- Title screen visible: yes/no
- Playable speed with static AOT: yes/no
- IWRAM heal leftovers: yes/no

Ruby USA
- SHA-1: f28b6ffc97847e94a6c21a63cacf633ee5c8df1e
- Title screen visible: yes/no/pending dump

Sapphire USA
- SHA-1: 3ccbbd45f8553c36463f13b938e833f652b793e4
- Title screen visible: yes/no/pending dump
```
