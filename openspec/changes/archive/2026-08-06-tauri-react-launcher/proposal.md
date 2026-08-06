## Why

The SDL3 custom-textured launcher looks amateur and is hard to polish. Players need a real product UI with a component library; a separate Tauri + React launcher keeps the C++ host focused on gameplay while giving us modern layout, covers, and actions.

## What Changes

- Add a **separate desktop launcher** (`gen3recomp-launcher` or equivalent) built with **Tauri 2 + React + a component library** (default: HeroUI v3 / Tailwind), floating compact window, cover grid, mouse-first Build / Play / Add ROM / Mods.
- **BREAKING (player default):** bare launch no longer uses the SDL textured grid as the primary UI. Default player entry opens the Tauri launcher; the C++ host runs the game session (spawned with `--rom` / `--bios` after gates pass).
- Reuse existing host capabilities: `roms/` scan + catalog SHA-1, BIOS check, cart AOT Build orchestration, mods enablement, cover cache under user-data (D7 — no shipping art).
- Keep `gen3recomp --rom` CLI and automation unchanged.
- Optionally keep a thin SDL launcher behind a flag or omit it from default docs once Tauri is the default (decide in design; prefer remove from default path, not dual-maintain forever).
- Document build for Linux (reference), Windows, and macOS for both host and Tauri app.
- Update product decisions as needed: D3 (launcher UI) and D10 (one product exe) — player install may ship **launcher + host** as the product surface.

### Non-goals

- Electron
- Rewriting the game loop / provider in web tech
- Shipping ROM/BIOS/covers/cart objects in git or Releases
- Thunderstore / in-game HUD redesign / FireRed–LeafGreen
- Full redesign of CLI flags

## Capabilities

### New Capabilities
- `tauri-launcher`: Separate Tauri + React shell that lists catalogued dumps with covers, surfaces BIOS/AOT/mods status, triggers Build, and starts the native host for Play.

### Modified Capabilities
- `launcher-ui`: Default bare-launch player UI is the Tauri app; SDL custom grid is no longer the required default experience.
- `launcher`: When `--rom` is omitted, startup MUST open the Tauri launcher path (directly or by documented player entrypoint), not only the SDL grid.
- `build-system`: Add Node/Rust/Tauri recipes alongside CMake host builds; document dual-artifact player packaging without violating D7.

## Impact

- New tree: `launcher/` (or `apps/launcher`) — Rust (Tauri) + React frontend; IPC/commands to scan ROMs, covers, AOT, mods, spawn host.
- Existing: `src/app/launcher_ui.cpp` demoted or removed from default; host stays C++20/CMake/SDL3 for gameplay.
- Dependencies: Node/npm, Rust toolchain, Tauri CLI; component library + Tailwind.
- Legal: D7 unchanged — covers remain user-data cache; Releases must not include dumps/BIOS/art/cart blobs.
- Assumption: HeroUI v3 (Beta) + React 19 + Tailwind v4 unless apply finds a blocking incompatibility; then swap to another documented React component library without changing capability requirements.
- Related: supersedes the visual approach of `launcher-gui-covers` for the player-facing shell (that change’s cover-cache semantics remain desirable and should be reused via host helpers or reimplemented in the Tauri backend).

Milestone: post-M08 player UX polish (see `openspec/product/roadmap.md` / D3).
