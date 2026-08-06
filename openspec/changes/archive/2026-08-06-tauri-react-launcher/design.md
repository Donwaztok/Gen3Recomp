## Context

See proposal.md — Why. Today the default bare-launch UI is a custom SDL3 textured grid (`launcher-gui-covers`). That path works functionally but looks amateur; polishing it in SDL without a real component system is costly. The gameplay host remains C++20 / CMake / SDL3. Cover cache, catalog SHA-1, AOT scripts, and mods already exist on the host side and should be reused, not reinvented in the game process.

Product decisions impacted: **D3** (player launcher UI) stays; **D10** (“one product exe”) is relaxed for the player surface to **launcher app + host exe**, with CLI still hitting `gen3recomp` alone. Update `openspec/product/decisions.md` and architecture notes at apply.

## Goals / Non-Goals

**Goals:**
- Ship a separate Tauri 2 + React launcher with a component library as the default player UI.
- Keep Play → spawn/invoke `gen3recomp --rom … --bios …` (or equivalent argv) after gates.
- Reuse cover cache policy (D7), `roms/` discovery, AOT Build orchestration, mods.
- Document dual build (CMake + Tauri) for Linux reference; Win/macOS recipes.

**Non-Goals:**
- Electron
- Embedding the full game renderer inside the WebView
- New top-level C++ modules (launcher is a sibling app tree, not `src/util`)
- Dual-maintaining polished SDL and Tauri UIs long-term

## Decisions

1. **Separate process (Tauri app) vs in-process ImGui**  
   → **Tauri**. User chose option 1; component libraries and layout quality beat ImGui for a “product” launcher.  
   **Rejected:** Electron (heavier); Dear ImGui in the host (still “tooling” look).

2. **UI stack: React + HeroUI v3 + Tailwind v4**  
   → Default for apply. If HeroUI v3 blocks shipping, swap to another React component library without changing specs.  
   **Rejected:** raw CSS-only; Vue/Svelte (team context already React/HeroUI-oriented).

3. **Repo layout: `launcher/` at repo root**  
   → `launcher/` with Tauri `src-tauri/` + Vite React frontend. Not a new C++ module.  
   **Rejected:** nesting under `src/app/` (wrong language/runtime).

4. **How bare `gen3recomp` behaves**  
   → Documented **player entrypoint is the Tauri binary**. Optionally: bare `gen3recomp` without `--rom` prints a short hint to run the launcher, or execs it if found beside the host — prefer **exec-if-found, else message** so one double-click story works when both are installed. CLI `--rom` unchanged.  
   **Rejected:** forever opening SDL grid as default.

5. **Backend logic in Tauri (Rust) calling host tools**  
   → Tauri commands: list ROMs (scan + SHA-1 vs catalog), BIOS status, cover resolve/fetch/cache, AOT ready check, run Build script, list/toggle mods, spawn host. Prefer invoking existing scripts/`gen3recomp` rather than reimplementing recomp in Rust. Small shared catalog/cover URL data can be read from `data/` files.  
   **Rejected:** HTTP localhost sidecar inside the C++ process for MVP.

6. **Covers**  
   → Same semantics as cover-art capability: curated URLs, user-data cache, placeholders, optional `roms/covers/`. Implement fetch in Tauri (reqwest/curl) or call a tiny host helper; do not commit blobs.

7. **Window chrome**  
   → Compact floating (~960×640 class), brand-forward library screen, mouse-first; keyboard optional. Follow project frontend design rules (no generic purple dashboard; one composition; covers as visual anchor).

## Risks / Trade-offs

- **[Risk] D10 / “one exe” messaging** → Mitigation: amend decisions.md; package as “player = launcher + host”.
- **[Risk] Toolchain weight (Node + Rust + C++)** → Mitigation: document clearly; CI optional for launcher on Linux first.
- **[Risk] HeroUI v3 beta churn** → Mitigation: pin versions; allow library swap without spec change.
- **[Risk] Path discovery (roms/, BIOS, host binary)** → Mitigation: search cwd, exe-adjacent, env override; surface errors in UI.
- **[Risk] SDL grid bitrots** → Mitigation: stop documenting it; delete or `#ifdef` diagnostic later.
- **[Trade-off] Two processes** → Cleaner UX vs slightly more packaging complexity.

## Migration Plan

1. Scaffold `launcher/` Tauri + React + HeroUI.
2. Wire Tauri commands to scan/catalog/BIOS/AOT/mods/covers.
3. Implement library UI (grid, gates, mods panel).
4. Play spawns host; Build shells to existing cart script.
5. Point README player flow at Tauri; update decisions D3/D10 notes.
6. Demote SDL launcher from default; keep CLI.
7. Manual checklist: covers online/offline, Build/Play Emerald on Linux.

Rollback: document SDL fallback or CLI-only; leave `launcher/` unshipped.

## Open Questions

- Exact binary name (`gen3recomp-launcher` vs productized name) — pick at apply; keep kebab-case in docs.
- Whether bare `gen3recomp` auto-execs the Tauri binary when present — default yes beside exe / on PATH; record final behavior in README at apply.
