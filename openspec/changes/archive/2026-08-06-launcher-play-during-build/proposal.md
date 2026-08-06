## Why

Cart AOT Build can take several minutes. Today the launcher treats Build as a global busy state, so the whole UI locks — the player cannot start a ready title while another dump is compiling. Waiting feels worse than it needs to, and progress lives only in the footer instead of on the cover that is actually building.

## What Changes

- Decouple Build from a global UI lock: while a cart AOT Build runs for one title, the user MUST still be able to Play other titles that are already AOT-ready (and meet BIOS/host gates).
- Keep **at most one** Build running at a time (second Build starts are refused or queued with a clear message). Concurrent multi-Build is a non-goal for this change (CPU/disk contention and artifact dirs).
- Show live Build progress on the **cover tile** of the title being built (bar + short phase), not only in the footer status strip.
- Disable only the in-progress title’s Build/Play (Play already gated by AOT); other tiles keep their normal actions.
- Progress events MUST identify which title/SHA-1 they belong to so the UI can bind the bar to the correct cover.

### Non-goals

- Running two (or more) cart AOT Builds in parallel
- Speeding up the Build itself
- Moving the game host into the launcher process
- Cancel/abort Build UI (MAY remain a follow-up)
- Changing Play spawn behavior beyond unlocking it during Build

### Assumption

“Uma build por vez” stays for resource safety; “livre pra jogar enquanto builda outro” means Play on a *different* ready title while Build runs. Progress on the cover is the primary indicator; footer may keep a compact echo or status line.

## Capabilities

### New Capabilities

_(none)_

### Modified Capabilities

- `launcher-ui`: Play remains available for other ready titles during Build; per-cover progress bar on the building tile; no global busy lock for Play/Build of unrelated titles.
- `tauri-launcher`: Build progress payloads include a title identity (e.g. SHA-1 or rom path); backend does not require the frontend to freeze other commands while Build runs (Play remains callable).

## Impact

- React (`launcher/src/App.tsx`, cover CSS): replace global `busy` during Build with per-rom build state; ProgressBar overlay on the building cover; Play/Build enablement per tile.
- Rust: include `sha1` (or equivalent) on `build-progress` events; optional guard rejecting a second concurrent `build_cart`.
- Specs: `launcher-ui`, `tauri-launcher` deltas.
- Relates to completed `launcher-build-progress` / cover action icons; milestone: Tauri launcher Build UX.
