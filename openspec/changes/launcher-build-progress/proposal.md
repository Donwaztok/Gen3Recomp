## Why

Cart AOT Build can take several minutes with no live feedback beyond a static status string. Players need a visible progress indicator so the wait feels bounded and they can see the build is still working.

## What Changes

- Stream Build progress from the cart artifact script into the Tauri launcher (line/phase events), instead of only returning the full log when the process exits.
- Show a **progress bar** (and short phase label) in the launcher UI while Build runs — determinate when shard/compile counts are known; otherwise a clear indeterminate/busy bar with phase text.
- Keep existing success/failure messaging and footer/cover Build entry points; progress MUST apply whether Build was started from the cover icon or the footer.
- Optionally emit machine-readable progress lines from `scripts/build_cart_artifact.sh` (e.g. `PROGRESS n/N phase=cc`) so the UI does not rely on fragile free-text scraping alone.

### Non-goals

- Speeding up the Build itself
- Progress for Play / host boot
- A full log console UI (a one-line phase + bar is enough; full log MAY remain on completion/error)
- Windows-first progress (Linux reference; keep Win/macOS from regressing if bash path exists)

### Assumption

“Barra de progresso” means a HeroUI (or equivalent) progress control with percent when available, plus a short phase string (`generating`, `compiling 12/400`, `linking`). Exact percent weights across phases may be approximate.

## Capabilities

### New Capabilities

_(none)_

### Modified Capabilities

- `launcher-ui`: While cart AOT Build runs, the UI MUST show a progress bar and phase/status text that updates before the command completes.
- `tauri-launcher`: Build orchestration MUST stream progress events (or equivalent) to the frontend; MUST NOT only report after the script exits.
- `build-system` *(optional / light)*: Document that the cart build script may emit progress lines consumed by the launcher.

## Impact

- Rust: `host::build_cart` → spawn + read stdout line-by-line; `app.emit("build-progress", …)`
- React: listen for events; ProgressBar during `busy` build; cover/footer Build share state
- Script: stable `PROGRESS` lines from compile/link loops
- Specs: `launcher-ui`, `tauri-launcher`; maybe a short note under `build-system`

Milestone: Tauri launcher Build UX.
