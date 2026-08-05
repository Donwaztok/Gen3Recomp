# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, or game assets.

## Status

Milestone M08: catalogued USA dumps launch through the isolated gba-recomp adapter into upstream native execution. Emerald USA is the first acceptance title; Ruby and Sapphire use the same binary and path.

The source of truth for what we will build is OpenSpec:

- Start here: [openspec/README.md](openspec/README.md)
- Vision: [openspec/product/vision.md](openspec/product/vision.md)
- Architecture: [openspec/product/architecture.md](openspec/product/architecture.md)
- Roadmap: [openspec/product/roadmap.md](openspec/product/roadmap.md)
- Decisions: [openspec/product/decisions.md](openspec/product/decisions.md)
- Manual boot checklist: [docs/manual-boot.md](docs/manual-boot.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

Later: FireRed and LeafGreen.

## Quick start

1. Install **SDL3** and **SDL2** (Arch/CachyOS: `sudo pacman -S sdl3 sdl2`).
2. Clone with submodules (gba-recomp lives in `third_party/gbarecomp`):

```sh
git clone --recurse-submodules <repo-url>
# existing checkout:
git submodule update --init --recursive
```

3. Build the host, then locally recompile **your** BIOS and ROM into native C++ (never committed), rebuild, and play:

```sh
cmake -S . -B build
cmake --build build --target gba_recompile gen3recomp
./scripts/recompile_user_bios.sh ./gba_bios.bin
./scripts/recompile_user_rom.sh "./Pokemon - Emerald Version (USA, Europe).gba"
cmake -S . -B build && cmake --build build -j"$(nproc)"
./build/gen3recomp --rom "./Pokemon - Emerald Version (USA, Europe).gba" --bios ./gba_bios.bin
ctest --test-dir build --output-on-failure
```

`--rom` is required. A GBA BIOS is required via `--bios` or `./gba_bios.bin`.

Without the local cart corpus under `generated/rom/`, the binary falls back to runtime self-heal (cold areas compile on first visit and feel like a few FPS). Static AOT covers the whole dump so gameplay after the name screen is already native.

Supported MVP dumps (exact SHA-1, USA): Ruby, Sapphire, and Emerald. Sources: [data/catalog_sources.md](data/catalog_sources.md). Unknown dumps are rejected with the computed SHA-1.

## Controls

Same layout as gba-recomp defaults: arrows = D-pad, **X** = A, **Z** = B, **Enter** = Start, **Right Shift** = Select, **C** = L, **V** = R. Esc or Q quits.

Cartridge saves live under `~/.local/share/gen3recomp/saves/<rom-sha1>.sav`. Save states are not part of the MVP.

A blank/white window usually means upstream is running without a locally recompiled BIOS (HLE boot skip), or SDL2's OpenGL backend is uploading RGB24 frames incorrectly (common on NVIDIA + Wayland). Generate BIOS sources from your dump, rebuild, and keep present-in-place enabled. The host defaults to `SDL_RENDER_DRIVER=software`; set that variable yourself to try `opengl` / `opengles2`.

IWRAM overlays (IRQ / m4a / AgbMain copies) still compile once into `~/.local/share/gen3recomp/recomp_cache/<rom-sha1>/`. `--prepare` only walks a fixed frame count and is not a substitute for static cart AOT.

Do not commit generated BIOS or ROM sources (`third_party/gbarecomp/src/runtime/generated_bios/`, `generated/rom/`).

## Intended user flow

```
gen3recomp
    → select a ROM
    → validate SHA-1
    → identify Ruby / Sapphire / Emerald
    → run the recompiler provider
    → start the runtime
    → the game runs natively
```

## License

**PolyForm Noncommercial License 1.0.0** — the same license as [gba-recomp](https://github.com/mstan/gbarecomp).

The source is public. Commercial use is not allowed. See [LICENSE](LICENSE) and [openspec/product/decisions.md](openspec/product/decisions.md) (D11).

Exit codes: `0` success, `1` input error, `2` usage error.
