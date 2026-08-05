# Vision

## Problem

Pokémon Generation III games still depend on emulation or fragmented experimental ports. Existing gba-recomp game repos prove that native recompilation can boot Ruby, Sapphire, and Emerald — but they are split across multiple repositories, multiple executables, and a framework-first workflow.

Players and contributors need one small, understandable, public noncommercial project that:

- runs the Gen3 games natively on desktop
- never ships Nintendo data
- stays simple enough for new contributors
- can replace its recompiler backend later without rewriting the host

## Product

**gen3recomp** is a single desktop application that:

1. Asks the user for a legally obtained ROM (and GBA BIOS).
2. Validates exact known dumps by SHA-1.
3. Identifies Pokémon Ruby, Sapphire, or Emerald automatically.
4. Prepares execution through an isolated recompiler provider.
5. Runs the original game logic natively against a GBA hardware runtime.
6. Presents video, audio, and input through a thin host layer (SDL3 first).

gen3recomp is a **host + catalog + provider seam**, not a Pokémon engine rewrite and not a new GBA emulator written from scratch.

## Success for MVP

MVP is successful when a contributor can clone the repo, build one executable, supply valid USA dumps of Ruby, Sapphire, or Emerald plus a valid GBA BIOS, and reach the game's title screen natively.

## Non-goals for this vision horizon

See [future.md](future.md). Multiplayer, Vulkan, mods, save states, plugin ABIs, editors, and networking are explicitly out of the MVP vision.

## Principles

- Keep it simple.
- One product binary.
- Title-specific data lives only in Game Definitions.
- Host presentation never knows Pokémon.
- Backend isolation over premature abstraction.
- Every milestone ships something runnable.
