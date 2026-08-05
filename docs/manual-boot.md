# Manual boot checklist

MVP acceptance is title-screen boot for USA Ruby, Sapphire, and Emerald at playable speed when static cart AOT is present.

## Prerequisites

- USA dump(s) under `roms/` (recommended) or any path — `*.gba` is gitignored
- GBA BIOS (`gba_bios.bin` or `--bios`)
- `git submodule update --init --recursive`
- SDL2 + SDL3

## Build (full-speed path)

```sh
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
./scripts/recompile_user_bios.sh /path/to/gba_bios.bin
./scripts/build_cart_artifact.sh /path/to/game.gba
cmake -DGEN3RECOMP_CART_ARTIFACT="$HOME/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/libcart.so" \
  -S . -B build
cmake --build build -j"$(nproc)" --target gen3recomp
ctest --test-dir build --output-on-failure
```

Configs: `data/emerald_usa.toml`, `data/ruby_usa.toml`, `data/sapphire_usa.toml` (auto-selected by ROM SHA-1).

**Dev link mode:** `./scripts/recompile_user_rom.sh` → `generated/rom/` → reconfigure without `GEN3RECOMP_CART_ARTIFACT` (multi-minute compile into the exe).

## Launch

```sh
./build/gen3recomp --rom /path/to/game.gba --bios /path/to/gba_bios.bin
```

Expect log lines: `static_cart=true` and `cart coverage=linked-artifact` (or `linked-generated-rom`). IWRAM PCs (`0x0300….`) may still heal once into `recomp_cache/`.

`--prepare` is optional/diagnostic only (self-heal warm-up). Do not use it as a substitute for cart AOT.

## Paths

| Kind | Location |
|------|----------|
| Cartridge save | `~/.local/share/gen3recomp/saves/<sha1>.sav` |
| Cart AOT `.so` | `~/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/libcart.so` |
| IWRAM / heal DLLs | `~/.local/share/gen3recomp/recomp_cache/<sha1>/` |

## Controls

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
gba-recomp pin: 2952aff2bb42f49de5903acf22af8fea3e2e3dee
Cart coverage: linked-artifact / linked-generated-rom / heal-only

Emerald USA
- SHA-1: f3ae088181bf583e55daf962a92bb46f4f1d07b7
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

## Notes

Upstream `run_game` owns the playable window (SDL2). Leave present-in-place enabled. Do not use `--frames --no-window` for acceptance.

## Local results (2026-08-05)

```
Date: 2026-08-05
Host: Linux
Binary: gen3recomp (static cart linked)
gba-recomp pin: 2952aff2bb42f49de5903acf22af8fea3e2e3dee
Cart coverage: linked-generated-rom (64 shards, kDispatchTableLen=137650)

Emerald USA
- SHA-1: f3ae088181bf583e55daf962a92bb46f4f1d07b7
- BIOS intro / HLE note: local BIOS recomp + --no-bios-hle; Flash1M; SDL software
- Title screen visible: yes (static_cart=true); IWRAM PCs healed (~62) under 0x0300….
- Audio / input: upstream SDL2 host map
- In-game save: ~/.local/share/gen3recomp/saves/<sha1>.sav

Ruby / Sapphire USA
- AOT configs landed; boot pending local dumps
```
