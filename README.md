# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, or game assets.

## Status

Milestone M08 plus local full-cart AOT and a player launcher UI: catalogued USA dumps run through the gba-recomp adapter. Emerald is validated with static cart coverage; Ruby and Sapphire use the same host and scripts.

OpenSpec: [openspec/README.md](openspec/README.md) · [docs/manual-boot.md](docs/manual-boot.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

Later: FireRed and LeafGreen.

## Player flow (recommended)

1. Install **SDL3** and **SDL2** (Arch/CachyOS: `sudo pacman -S sdl3 sdl2`) plus a C++ toolchain (`c++` on PATH) for the one-time cart Build.
2. Clone with submodules: `git submodule update --init --recursive`
3. Build the host + recompiler tool, then BIOS AOT:

```sh
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
./scripts/recompile_user_bios.sh ./gba_bios.bin
```

4. Put catalogued dumps in `roms/` and `gba_bios.bin` in the working directory (all gitignored).
5. Run the launcher (no `--rom`):

```sh
./build/gen3recomp
```

In the launcher: select a title → **B** Build (once, may take minutes) → **Enter** Play. The stock binary **dlopens** `~/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/libcart.so` — no CMake relink for players.

**Mods:** place packages under `mods/<id>/mod.toml` (or `~/.local/share/gen3recomp/mods/`). Press **M** in the launcher to enable/disable. Packages must not embed `.gba` / BIOS payloads. In-game mod toggles are not available yet (launcher-only).

## CLI (automation)

```sh
./build/gen3recomp --rom "./roms/Pokemon - Emerald Version (USA, Europe).gba" --bios ./gba_bios.bin
```

`--rom` selects the CLI path (skips the launcher). BIOS via `--bios` or `./gba_bios.bin`.

**Dev shortcut:** `./scripts/recompile_user_rom.sh <rom>` writes `generated/rom/` and CMake can compile those shards into the exe (slow every clean build). Prefer the cart artifact + runtime load for day-to-day host rebuilds. Optional `-DGEN3RECOMP_CART_ARTIFACT=…` still links the `.so` at build time for contributors.

Without cart AOT, the binary uses empty cart dispatch + self-heal (cold ROM PCs feel like a few FPS). `--prepare` only warms heal cache for a frame window — optional diagnostic, not the product path.

**Release zip (D7):** host + tools/scripts — never ROM/BIOS or prebuilt cart objects. Users supply dumps and run Build locally.

Supported MVP dumps (exact SHA-1): [data/catalog_sources.md](data/catalog_sources.md).

## Controls

**Launcher:** Up/Down select, **A** Add ROM…, **B** Build, **Enter/X** Play, **M** Mods, **R** Refresh, Esc/Q quit.

**In-game:** Arrows = D-pad, **X** = A, **Z** = B, **Enter** = Start, **Right Shift** = Select, **C** = L, **V** = R. Esc or Q quits.

Saves: `~/.local/share/gen3recomp/saves/<rom-sha1>.sav`  
Cart artifact: `~/.local/share/gen3recomp/cart_aot/<rom-sha1>/abi3-linux-x64/libcart.so`  
IWRAM heal cache: `~/.local/share/gen3recomp/recomp_cache/<rom-sha1>/`

Blank/white window: usually missing BIOS recomp or OpenGL RGB24 on NVIDIA/Wayland — keep `SDL_RENDER_DRIVER=software` (host default) and local BIOS AOT.

## License

**PolyForm Noncommercial License 1.0.0** — same as [gba-recomp](https://github.com/mstan/gbarecomp). See [LICENSE](LICENSE).

Exit codes: `0` success, `1` input error, `2` usage error.
