## Context

See proposal.md for motivation. Today `App.tsx` sets a global `busy` flag for the entire Build (`setBusy(true)` for minutes), which disables cover/footer Play and Build for every tile. Progress from `build-progress` is rendered only in the footer strip. `build_cart` is already async + `spawn_blocking` (UI freeze fix); this change is about concurrency UX and per-tile progress binding.

Build artifacts are per-SHA-1 under the cart cache; Play spawns a separate `gen3recomp` host process — so Play-during-Build is safe at the process model level. Parallel Builds are not, because compile jobs compete for cores/disk and the script/workdir assumptions are single-job oriented.

## Goals / Non-Goals

**Goals:**

- Per-ROM build state in the UI (not global busy for Build)
- Cover-mounted progress for the building SHA-1
- Play other ready titles during Build
- Backend: keyed progress events + single-flight Build guard

**Non-Goals:**

- Parallel Builds
- Build cancel button
- Changing host spawn / AOT pipeline internals

## Decisions

1. **Single-flight Build (frontend + backend)**  
   - **Choice:** Track `buildingSha1` (or map of one entry) in React; reject a second `build_cart` in Rust with a mutex/`AtomicBool` or `tokio` lock held for the duration of the spawn_blocking job.  
   - **Why:** Spec says at most one Build; backend guard prevents races if UI is bypassed.  
   - **Rejected:** Queue of Builds (extra UX); N parallel Builds (resource thrash).

2. **Progress payload includes `sha1`**  
   - **Choice:** Extend `BuildProgressEvent` with `sha1: String` (resolved from the ROM path via existing identify/catalog helpers before/during build). Emit on every `build-progress` event.  
   - **Why:** Cover binding must not depend on “currently selected” tile (user can select another title while building).  
   - **Rejected:** Key only by rom path (normalization pain); key by selection index (breaks on refresh).

3. **Global `busy` only for short ops**  
   - **Choice:** Keep `busy` for refresh/add/identify/play-spawn handshake if needed briefly; Build uses `buildingSha1` instead. Disable Build on all tiles while `buildingSha1 != null`; disable Play only on the building tile (already not `aot_ready`) and during short `busy` for Play’s own invoke.  
   - **Why:** Matches “play while build” without inventing a full job system.  
   - **Rejected:** Leaving `busy` true for whole Build (current bug).

4. **Cover progress UI**  
   - **Choice:** Overlay a compact HeroUI `ProgressBar` (or thin bar) on the building tile’s cover, bottom strip under/near the action icon; optional short phase text. Footer may keep a one-line echo for accessibility/status.  
   - **Why:** User asked for progress on the cover when Play-during-Build is allowed; footer-only is easy to miss when focusing another tile.  
   - **Rejected:** Progress only on footer; modal blocking dialog.

5. **Refresh after Build**  
   - **Choice:** On Build completion, refresh library (or patch that ROM’s `aot_ready`) without setting global busy that blocks Play mid-session start. Prefer a quiet refresh (`fetchCovers: false`) like today.  
   - **Why:** Unlock Play on the finished tile without freezing the grid.

## Risks / Trade-offs

- **[Risk]** Heavy compile load while playing → host FPS dips  
  → **Mitigation:** Acceptable for MVP; document; optional later niceness (lower `jobs` while host running) is out of scope.

- **[Risk]** Progress events without `sha1` from older builds  
  → **Mitigation:** Only one binary ships; always set `sha1` when emitting.

- **[Risk]** User selects another tile; footer status confusing  
  → **Mitigation:** Cover is primary; footer message can include display name.

## Migration Plan

- Ship with launcher rebuild; no data migration.
- Rollback: revert UI busy split and event field (optional field ignored by old UI).

## Open Questions

_(none blocking — parallel Builds deferred by proposal non-goal)_
