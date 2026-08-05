# Manual boot checklist

MVP acceptance is title-screen boot for USA Ruby, Sapphire, and Emerald. Automate only stable seams; this checklist is the product gate.

## Prerequisites

- A legally obtained USA dump of the title under test
- A legally obtained GBA BIOS (`gba_bios.bin` or `--bios`)
- Initialized gba-recomp submodule in `third_party/gbarecomp` (`git submodule update --init --recursive`)
- SDL2 (upstream host window/audio/input) and SDL3 (gen3recomp null-backend / host modules)

## Build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Launch

```sh
./build/gen3recomp --rom /path/to/game.gba --bios /path/to/gba_bios.bin
```

Cartridge saves: `~/.local/share/gen3recomp/saves/<rom-sha1>.sav`  
Self-heal cache: `~/.local/share/gen3recomp/recomp_cache/<rom-sha1>/`

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

Save states are not an MVP feature. Use in-game cartridge saves only.

## Results template

Copy this block into a local note or PR description. Do not commit ROM dumps.

```
Date:
Host:
Binary: gen3recomp
gba-recomp pin: 2952aff2bb42f49de5903acf22af8fea3e2e3dee

Emerald USA
- SHA-1:
- BIOS intro / HLE note:
- Title screen visible: yes/no
- Audio heard: yes/no/muted
- Input reaches title menu: yes/no
- In-game save + reload: yes/no/skipped

Ruby USA
- SHA-1:
- Title screen visible: yes/no
- In-game save + reload: yes/no/skipped

Sapphire USA
- SHA-1:
- Title screen visible: yes/no
- In-game save + reload: yes/no/skipped
```

## Notes

Upstream `run_game` owns the playable window (SDL2), audio device, and keyboard mapping during native boot. gen3recomp SDL3 audio/input modules stay available for the null backend and for a later steppable unwrap.

Without a locally generated BIOS recompilation linked into gba-recomp, upstream forces BIOS HLE and skips the Nintendo logo intro. Emerald often stays on a white screen in that mode. Leave upstream present-in-place enabled (do not set `GBARECOMP_PRESENT_IN_PLACE=0`) so VBlank presents resume in-place instead of redispatching mid-function PCs. On NVIDIA/Wayland, SDL2 OpenGL may also present a solid white window even while the PPU is drawing the BIOS intro; gen3recomp defaults to the SDL software renderer (`SDL_RENDER_DRIVER=software`). Generate BIOS sources locally, rebuild, then relaunch:

```sh
./scripts/recompile_user_bios.sh ./gba_bios.bin
cmake -S . -B build && cmake --build build
```

Do not commit those generated files. If a previous self-heal cache looks stuck, delete `~/.local/share/gen3recomp/recomp_cache/<sha1>/` and try again.

Do not use upstream `--frames --no-window` for acceptance; that path currently aborts in the pin. Launch the windowed binary instead.

## Local results (2026-08-05)

```
Date: 2026-08-05
Host: Linux
Binary: gen3recomp
gba-recomp pin: 2952aff2bb42f49de5903acf22af8fea3e2e3dee

Emerald USA
- SHA-1: f3ae088181bf583e55daf962a92bb46f4f1d07b7
- BIOS intro / HLE note: local BIOS recomp + `--no-bios-hle` (LLE intro); Flash1M; SDL software renderer shows Nintendo / Game Boy logos
- Title screen visible: after logos → copyright → white fade → Pokémon logo; first boot compiles IWRAM/ROM shards (can take a minute; do not Ctrl+C)
- Audio heard: routed through upstream SDL2 host
- Input reaches title menu: upstream keyboard map (X/Z/Enter/…)
- In-game save + reload: save file keyed at ~/.local/share/gen3recomp/saves/<sha1>.sav; in-game round-trip is manual

Ruby USA
- SHA-1: same host path / binary; dump not present on this machine
- Title screen visible: pending local dump
- In-game save + reload: pending

Sapphire USA
- SHA-1: same host path / binary; dump not present on this machine
- Title screen visible: pending local dump
- In-game save + reload: pending
```
