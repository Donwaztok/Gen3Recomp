## Context

`friendly-launcher-aot-mods` shipped a working AOT gate and mods, but the UI is `SDL_RenderDebugText` + keyboard only. Players want a floating, mouse-driven cover grid. Covers may be obtained from Cover Project / Wikipedia, but D7 forbids shipping Nintendo assets in git or Releases—so fetch-to-user-cache is the only compliant approach.

Cross-platform: the host already targets SDL3; cart AOT today is Linux-tagged (`abi3-linux-x64`). This change makes the **launcher/host compile and present** on Win/Linux/macOS; cart artifact scripts per OS are included so Play/Build remain meaningful where toolchains exist.

## Goals / Non-Goals

**Goals:**
- Floating compact launcher window with cover **grid**, **mouse** select/actions, polished Build/Play/Mods/Add ROM.
- Curated HTTPS cover fetch → user-data cache; placeholders offline.
- Documented builds for Windows, Linux, macOS.

**Non-Goals:**
- Qt/Electron; bundling cover PNGs in Releases; FireRed/LeafGreen; full game-store chrome; rewriting gba-recomp to SDL3.

## Decisions

### D-GUI-1 — SDL3 textured UI (not Qt)
Keep SDL3. Implement grid with textures + hit-testing (custom immediate UI or Dear ImGui). Prefer **custom textured UI** if ImGui would fight the existing `sdl3_dyn` / SDL2 coexistence; otherwise ImGui via FetchContent is acceptable if apply proves clean.

**Rejected:** Electron/Qt; keeping debug-text as the product UI.

### D-GUI-2 — Floating window
Default window: compact (~960×640 class), resizable, **not** maximized/fullscreen. Normal desktop window (not always-on-top unless a later toggle). Title “gen3recomp”.

**Rejected:** Kiosk/fullscreen launcher; system tray-only.

### D-GUI-3 — Covers: curated URLs → user-data cache
Catalog (or `data/covers.toml`) maps title id / SHA-1 → primary Wikimedia file URL and optional Cover Project fallback. On first need: HTTPS GET → `~/.local/share/gen3recomp/covers/<id>.(png|jpg)` (XDG / AppData / macOS Application Support via existing user-data helpers). Optional local override: `roms/covers/<id>.png` (gitignored pattern).

**Legal stance (D7):** never commit or Release cover blobs. Document that covers are third-party copyrighted works cached locally for identification UX, analogous to user-supplied ROMs. Prefer Wikimedia URLs with stable file names.

**Rejected:** Shipping art in repo; unrestricted HTML scraping; blocking Play when art missing.

### D-GUI-4 — Network is optional
Fetch is best-effort on a background thread; UI never freezes. Failures → placeholder tile (gradient + display name + AOT badge).

### D-GUI-5 — Cross-platform host
CMake + SDL3 on Win/Linux/macOS. Extend `cart_artifact_abi_tag()` and build scripts with `abi3-windows-x64` / `abi3-macos-*` stubs or implementations so paths are not Linux-hardcoded in the UI. Linux remains the primary validated AOT path; Win/macOS Build may surface “toolchain required” clearly if scripts are incomplete.

**Rejected:** Linux-only launcher forever; requiring users to build only under WSL for the GUI itself.

### Dependency direction
```
app (floating grid launcher)
  → cover-art helper (resolve/fetch/cache)
  → game (catalog + cover URL metadata)
  → platform (SDL3 window, mouse, textures via sdl3_dyn)
  → recomp (readiness, build, prepare) — unchanged contracts
```

## Risks / Trade-offs

- **[Risk] Cover URL rot / ToS** → Mitigation: curated map in-repo (URLs only); dual source; placeholders; easy URL bump.
- **[Risk] Copyright complaints** → Mitigation: cache-only, no redistribution; document policy; allow delete-cache.
- **[Risk] SDL2/SDL3 coexistence** → Mitigation: keep `sdl3_dyn` for all launcher/platform SDL3 calls.
- **[Risk] Win/macOS AOT incomplete** → Mitigation: UI builds everywhere; Build button errors clearly; Linux AOT remains reference.
- **[Trade-off] Custom UI vs ImGui** → Chose custom SDL3 textured grid at apply (ImGui skipped to avoid SDL2/SDL3 coexistence with `sdl3_dyn`).

## Migration Plan

1. Add cover URL metadata + cache/fetch helper.
2. Rebuild launcher UI (grid + mouse + floating window).
3. Wire AOT/BIOS/mods/actions into new chrome.
4. Multi-OS CMake/docs + ABI tag plumbing.
5. Manual checklist: covers online/offline; click Play Emerald on Linux.

Rollback: keep CLI `--rom`; feature-flag or revert launcher_ui.cpp to text UI if needed.

## Open Questions

- Exact ImGui vs custom — **resolved at apply:** custom SDL3 textured grid (keeps `sdl3_dyn`, avoids ImGui + SDL2 coexistence risk).
- Whether Cover Project requires attribution UI string — show footer “Covers cached locally (D7)” for now; add source-specific credit if terms require when Cover Project URL is used.
