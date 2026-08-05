# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, or game assets.

## Status

Milestone M08 plus local full-cart AOT: catalogued USA dumps run through the gba-recomp adapter. Emerald is validated with static cart coverage; Ruby and Sapphire use the same host and scripts.

OpenSpec: [openspec/README.md](openspec/README.md) · [docs/manual-boot.md](docs/manual-boot.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

Later: FireRed and LeafGreen.

## ROMs and BIOS (local only)

Place dumps anywhere; recommended layout (all `*.gba` / BIOS patterns are gitignored):

```text
roms/
  Pokemon - Ruby Version (USA).gba
  Pokemon - Sapphire Version (USA).gba
  Pokemon - Emerald Version (USA, Europe).gba
gba_bios.bin
```

Never commit ROMs, BIOS, `generated/`, or `~/.local/share/gen3recomp/` artifacts.

## Quick start (full-speed)

1. Install **SDL3** and **SDL2** (Arch/CachyOS: `sudo pacman -S sdl3 sdl2`).
2. Clone with submodules: `git submodule update --init --recursive`
3. Build tools + BIOS AOT, then **one-time cart artifact**, then link the host:

```sh
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
./scripts/recompile_user_bios.sh ./gba_bios.bin
./scripts/build_cart_artifact.sh "./roms/Pokemon - Emerald Version (USA, Europe).gba"
# prints GEN3RECOMP_CART_ARTIFACT=.../libcart.so
cmake -DGEN3RECOMP_CART_ARTIFACT="$HOME/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/libcart.so" \
  -S . -B build
cmake --build build -j"$(nproc)" --target gen3recomp
./build/gen3recomp --rom "./roms/Pokemon - Emerald Version (USA, Europe).gba" --bios ./gba_bios.bin
```

`--rom` is required. BIOS via `--bios` or `./gba_bios.bin`.

**Dev shortcut:** `./scripts/recompile_user_rom.sh <rom>` writes `generated/rom/` and CMake can compile those shards into the exe (slow every clean build). Prefer the cart artifact for day-to-day host rebuilds.

Without cart AOT, the binary uses empty cart dispatch + self-heal (cold ROM PCs feel like a few FPS). `--prepare` only warms heal cache for a frame window — optional diagnostic, not the product path.

Supported MVP dumps (exact SHA-1): [data/catalog_sources.md](data/catalog_sources.md).

## Controls

Arrows = D-pad, **X** = A, **Z** = B, **Enter** = Start, **Right Shift** = Select, **C** = L, **V** = R. Esc or Q quits.

Saves: `~/.local/share/gen3recomp/saves/<rom-sha1>.sav`  
Cart artifact: `~/.local/share/gen3recomp/cart_aot/<rom-sha1>/abi3-linux-x64/libcart.so`  
IWRAM heal cache: `~/.local/share/gen3recomp/recomp_cache/<rom-sha1>/`

Blank/white window: usually missing BIOS recomp or OpenGL RGB24 on NVIDIA/Wayland — keep `SDL_RENDER_DRIVER=software` (host default) and local BIOS AOT.

## License

**PolyForm Noncommercial License 1.0.0** — same as [gba-recomp](https://github.com/mstan/gbarecomp). See [LICENSE](LICENSE).

Exit codes: `0` success, `1` input error, `2` usage error.
