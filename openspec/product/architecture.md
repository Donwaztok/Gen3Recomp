# Architecture

## One-sentence shape

`gen3recomp` is a small C++ host: **App** obtains a ROM, **Game** identifies it, **Recomp** prepares native execution through an isolated provider, **Runtime** runs a session, and **Platform / Video / Audio / Input** present a GBA-shaped host surface.

## User flow

```
gen3recomp [--rom PATH] [--bios PATH]
        │
        ▼
   App (launcher)
        │  ROM path + BIOS path
        ▼
   Game identification
        │  SHA-1 → Game Definition or reject
        ▼
   Recomp provider (gba-recomp adapter first)
        │  session artifact / prepared backend
        ▼
   Runtime session
        │  ticks hardware + native/JIT/interpreter code
        ├─────────────┬──────────────┐
        ▼             ▼              ▼
     Video         Audio          Input
        └─────────────┴──────────────┘
                      ▼
                 Platform (SDL3)
```

## Modules

Keep the count low. These are **internal static libraries** (or equivalent source groups) linked into **one** product executable.

| Module | Responsibility | Must not do |
|--------|----------------|-------------|
| `core` | Types used across modules, errors, logging setup, SHA-1, config/user-data paths, file read helpers | Pokémon rules, SDL, gba-recomp includes |
| `app` | Process entry orchestration, CLI, optional file dialog, exit codes, human error text | Hardware loop, title-specific logic |
| `game` | Game Definition data and catalog lookup by SHA-1 | Runtime scheduling, rendering, provider APIs |
| `recomp` | Provider contract + `gba` adapter only | SDL, Pokémon presentation, host UI |
| `runtime` | Session lifecycle and frame/audio/input pump | Emerald/Ruby/Sapphire special cases |
| `platform` | SDL3 window, events, time, dialogs | Pokémon, GBA PPU/APU, recompiler APIs |
| `video` | Present a framebuffer (later: palettes/tiles/sprites if exposed) | Game logic, Pokémon names, save formats |
| `audio` | Submit PCM samples to the host device | M4A/HLE game audio rewriting |
| `input` | Map host keyboard/gamepad to GBA buttons | Cheat/mod/editor features |

Rejected as standalone modules: `util`, `filesystem`, `tests-as-product-module`. See [decisions.md](decisions.md) D1.

## Logical source layout (target)

```
src/
  main.cpp                 thin main → app
  core/
  app/
  game/
  recomp/
    gba/                   ONLY translation unit tree allowed to touch gba-recomp
  runtime/
  platform/
  video/
  audio/
  input/
tests/                     Catch2
third_party/gbarecomp/     pinned upstream submodule, not wrapped casually elsewhere
```

This layout is specified now and created in M01–M02. Do not invent extra directories during MVP.

## Dependency rules

Allowed direction (↓ depends on):

```
main → app → runtime → recomp
                 │  ↘
                 │    game
                 ↓
         video / audio / input
                 ↓
              platform
                 ↓
                core

game     → core
recomp   → core, game
platform → core
video    → core, platform
audio    → core, platform
input    → core, platform
```

Forbidden:

- `runtime` → title-specific branches (`if emerald …`)
- `platform` / `video` / `audio` / `input` → `game` or `recomp`
- any module outside `recomp/gba` → gba-recomp headers or generated ROM C++
- circular dependencies

## Game Definition

A Game Definition is **data**, not a subclass hierarchy.

It carries only what the host and provider need:

- stable id (`ruby-usa`, `sapphire-usa`, `emerald-usa`)
- display name
- region
- exact SHA-1 (or a small set of accepted SHA-1s if a title has equivalent dumps — MVP starts with one canonical hash per title)
- provider hints (save family, RTC present, symbol/metadata id, generation family `gen3`)
- no executable logic

Ruby, Sapphire, and Emerald share one Gen3 family profile. Differences are catalog fields, not duplicated runtimes.

The runtime receives an opaque `GameDefinition` and must behave the same way for every title. Title differences are applied by the provider when preparing the backend, using those fields.

## Recompiler seam

Two small contracts, one adapter in MVP:

1. **Recompiler provider** — given ROM bytes + Game Definition (+ BIOS as required), prepare an execution artifact.
2. **Session backend** — start, step/run, stop against host video/audio/input services.

The gba-recomp adapter may implement both. Callers still depend only on the seam.

Why two contracts instead of one mega-interface: replacing “how code is produced” should not force replacing “how a frame is stepped”, and vice versa. Keep the contracts tiny. No plugin ABI, no shared-library discovery, no registry framework. Wiring is compile-time.

### What gba-recomp owns

Upstream already provides:

- ARM/Thumb decode, static recompile, interpreter, in-process JIT healing
- GBA bus, PPU, audio, DMA, timers, IRQ
- cartridge save chips, RTC
- BIOS execution path
- some host glue

gen3recomp must **not** clone that stack.

### What gen3recomp owns

- unified catalog and validation
- one executable / one UX flow for all MVP titles
- host presentation adapters
- isolation so upstream can be upgraded or replaced

## Presentation model

Video presents what the backend emits. MVP presentation is a **framebuffer blit**.

Do not build a Pokémon-aware sprite engine. If later we expose tiles/palettes/windows, they remain generic GBA primitives inside `video`.

Audio is PCM out. Input is GBA button state in.

## Platform seam

Only SDL3 is implemented. Name the platform boundary so a future backend can exist, but do not create a second implementation or a factory maze.

## Build shape

- CMake, C++20
- one product target: `gen3recomp`
- one test target: `gen3recomp_tests`
- third-party via CMake (FetchContent or pinned submodule)
- no installable intermediate product libraries

## Execution model (MVP)

We cannot ship ROM-derived generated C++. Therefore MVP execution is **user-ROM-in, local prepare, then run**.

Preferred path, because gba-recomp already has it:

- interpret missing code
- JIT-heal in process
- persist a local cache under the user data directory, keyed by ROM identity

Optional later: generate C++ on the user’s machine if a toolchain is present. Not required for MVP boot.

## Error model

Fail fast and loudly.

- unknown ROM → non-zero exit, message includes SHA-1 and “unsupported dump”
- missing/invalid BIOS → non-zero exit, no boot
- provider failure → non-zero exit, no partial windowed “maybe it works”

No silent fallback that pretends a random GBA ROM is a supported title.
