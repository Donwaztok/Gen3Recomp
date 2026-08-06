## 1. Backend progress identity and single-flight Build

- [x] 1.1 Add `sha1` (or equivalent dump id) to `BuildProgressEvent` and set it on every `build-progress` emit from `build_cart`
- [x] 1.2 Resolve SHA-1 from the ROM path before/at Build start (reuse existing identify/hash helpers)
- [x] 1.3 Guard `build_cart` so a second concurrent Build returns a clear error without starting another process

## 2. Frontend per-tile build state

- [x] 2.1 Replace Build’s use of global `busy` with per-dump state (`buildingSha1` + progress keyed by sha1)
- [x] 2.2 Keep Play enabled for other AOT-ready titles while `buildingSha1` is set; disable only Build globally (or non-building tiles’ Build) and the building tile’s Play via AOT gate
- [x] 2.3 Bind `build-progress` listener to update progress for the event’s `sha1`, not only the selected tile
- [x] 2.4 On Build success/failure, clear that dump’s in-progress state and refresh/patch `aot_ready` without locking the whole grid

## 3. Cover progress UI

- [x] 3.1 Render a ProgressBar (and short phase text) on the cover of the dump currently building
- [x] 3.2 Ensure footer/cover Build entry points share the same per-dump state; optional footer status echo with display name
- [x] 3.3 Style so the bar does not block cover selection or confuse action icons

## 4. Validate

- [x] 4.1 Rebuild the Tauri launcher (`npm run tauri:build` with project `CARGO_TARGET_DIR`)
- [x] 4.2 Manual check: start Build on one title, Play another ready title; confirm cover bar updates and second Build is blocked
