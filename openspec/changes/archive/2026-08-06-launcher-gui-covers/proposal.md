## Why

The current launcher is a debug-text keyboard list. Players expect a real floating GUI: cover art grid, mouse clicks, and a polished Build/Play/Mods flow—while staying legal under D7 (no shipping Nintendo assets in git/Releases).

## What Changes

- Replace the `SDL_RenderDebugText` list with a **visual floating launcher**: compact free-floating window (not maximized), mouse-first (click to select / Play / Build / Mods / Add ROM), keyboard still works.
- Show catalogued titles in a **cover grid**. Covers are **fetched on demand** from documented public sources (prefer Wikimedia Commons / Wikipedia file URLs; Cover Project as fallback) into **user-data cache only**—never committed or shipped in Releases.
- Offline / fetch failure: show a **styled placeholder** (title + status), never block listing or Play solely because art is missing.
- Keep existing AOT gate, BIOS status, `roms/` discovery, Add ROM…, and mods panel—surfaced in the new UI.
- **Cross-platform host build**: the launcher (and stock `gen3recomp` shell) MUST compile for **Windows, Linux, and macOS** via the documented CMake path. Cart AOT artifact ABI remains platform-specific (Linux path already exists; Win/macOS artifact tags/scripts are in scope as stubs or first implementations so the UI builds and runs on each OS).
- Preserve CLI `--rom` / `--bios` unchanged.

## Capabilities

### New Capabilities
- `cover-art`: Resolve, download (optional network), cache, and display per-title cover images for the launcher grid without shipping Nintendo art in the distribution.

### Modified Capabilities
- `launcher-ui`: Visual floating grid UI with mouse interaction; supersedes debug-text list as the default bare-launch experience.
- `launcher`: Bare launch still opens launcher UI; behavior unchanged aside from UI presentation.
- `build-system`: Document and ensure host builds for Windows, Linux, and macOS (CI matrix or equivalent local recipes).

## Impact

- Modules: primarily `app` (launcher UI), `platform` (mouse, textures, floating window), thin helpers for HTTP fetch/cache under `core` or `app` (no new top-level module unless design requires architecture.md update).
- Legal: D7 unchanged—covers are local cache from third-party URLs; Releases zip must not include cover blobs. User must have network for first fetch (or supply optional local override files).
- Network: first-time cover fetch requires HTTPS; failures degrade to placeholders.
- Non-goals: Qt/Electron shell; Thunderstore; shipping official art in the repo; fullscreen “store” chrome; FireRed/LeafGreen catalog; redesigning in-game HUD.

Assumptions: SDL3 remains the UI stack (ImGui or custom textured UI—decide in design); Cover Project / Wikipedia URLs are curated in catalog metadata, not scraped ad hoc at runtime; “floating” means a compact normal/tool window, not a kiosk.
