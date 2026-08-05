# gen3recomp

Native recompilation host for **Pokémon Generation III**.

The user supplies their own legally obtained Game Boy Advance ROM (and BIOS). This project does **not** distribute Nintendo ROMs, BIOS images, or game assets.

## Status

Milestone M01: the project builds a `gen3recomp` executable that reports its version.

The source of truth for what we will build is OpenSpec:

- Start here: [openspec/README.md](openspec/README.md)
- Vision: [openspec/product/vision.md](openspec/product/vision.md)
- Architecture: [openspec/product/architecture.md](openspec/product/architecture.md)
- Roadmap: [openspec/product/roadmap.md](openspec/product/roadmap.md)
- Decisions: [openspec/product/decisions.md](openspec/product/decisions.md)

## MVP titles

- Pokémon Ruby (USA)
- Pokémon Sapphire (USA)
- Pokémon Emerald (USA)

Later: FireRed and LeafGreen.

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

## Build (Linux)

```sh
cmake -S . -B build
cmake --build build
./build/gen3recomp --version
./build/gen3recomp --help
./build/gen3recomp --rom /path/to/game.gba
ctest --test-dir build --output-on-failure
```

`--rom` is required in this milestone. A file picker can wait until the SDL3 window exists.

Exit codes: `0` success, `1` input error, `2` usage error.
