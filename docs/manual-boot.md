# Manual boot checklist

MVP acceptance is title-screen boot for USA Ruby, Sapphire, and Emerald at playable speed when static cart AOT is present.

## Prerequisites

- USA dump(s) under `roms/` (recommended) or any path — `*.gba` is gitignored
- GBA BIOS (`gba_bios.bin` or `--bios`)
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
3. Press **B** Build once if AOT is missing (progress in status line)
4. Press **Enter** Play when unlocked
5. Expect log: `activated cart artifact via dlopen` and `coverage=runtime-loaded-artifact` (or linked-* for dev builds)

## CLI path

```sh
./scripts/build_cart_artifact.sh /path/to/game.gba   # optional if built from UI
./build/gen3recomp --rom /path/to/game.gba --bios /path/to/gba_bios.bin
```

Stock hosts dlopen the user-data `libcart.so`. Optional contributor link: `-DGEN3RECOMP_CART_ARTIFACT=…`.

Configs: `data/emerald_usa.toml`, `data/ruby_usa.toml`, `data/sapphire_usa.toml` (auto-selected by ROM SHA-1).

**Dev link mode:** `./scripts/recompile_user_rom.sh` → `generated/rom/` → reconfigure without cart artifact (multi-minute compile into the exe).

`--prepare` is optional/diagnostic only (self-heal warm-up). Do not use it as a substitute for cart AOT.

## Manual checklist (this change)

- [ ] Bare `./build/gen3recomp` opens launcher (not usage-only)
- [ ] Multiple catalogued dumps listed; unknown dump rejected on Add
- [ ] Emerald Build → Play boots to title at playable speed
- [ ] Mod enable/disable persists across launcher reopen
- [ ] In-game mod toggle: N/A (launcher-only; documented)

## Paths

| Kind | Location |
|------|----------|
| Cartridge save | `~/.local/share/gen3recomp/saves/<sha1>.sav` |
| Cart AOT `.so` | `~/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/libcart.so` |
| IWRAM / heal DLLs | `~/.local/share/gen3recomp/recomp_cache/<sha1>/` |
| Mod enablement | `~/.local/share/gen3recomp/mods_enabled.txt` |

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
