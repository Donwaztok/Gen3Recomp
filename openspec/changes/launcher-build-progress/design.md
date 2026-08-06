## Context

See proposal.md — Why. Today `host::build_cart` uses `Command::output()` and the React UI only shows a static “Building…” string until the full log returns. `build_cart_artifact.sh` already prints phases (`==> compiling N shards`, `cc $base`, `==> linking`).

## Goals / Non-Goals

**Goals:**
- Stream stdout → Tauri events → ProgressBar + phase label
- Determinate bar when N shards known
- Same UX for cover and footer Build

**Non-Goals:**
- Faster compiles; full log viewer; Play progress

## Decisions

### D1 — Transport
- **Choice:** `tauri::Emitter` event e.g. `build-progress` with `{ phase, current, total, message }`; finish via command `Result` as today (or a final event + result).
- **Why:** Fits Tauri 2; avoids polling.

### D2 — Script contract
- **Choice:** Add optional lines `PROGRESS current=<i> total=<n> phase=<id>` (and maybe `phase=generate|compile|link`) while keeping existing `echo` human lines.
- **Why:** Stable parse; CLI users ignore them.

### D3 — Percent mapping
- **Choice:** Weight roughly generate 10%, compile 80% (shard i/N), link 10% — tune in apply.
- **Why:** Compile dominates wall time.

### D4 — UI
- **Choice:** HeroUI `ProgressBar` (or ProgressCircle) near footer status / modal strip while `building`; disable duplicate Build clicks while busy.
- **Why:** Matches existing HeroUI shell.

### D5 — Windows
- **Choice:** Linux bash path first; if Windows lacks bash, keep error as today without inventing a second progress pipeline in this change.
- **Why:** Reference platform is Linux.

## Risks / Trade-offs

- [Stdout buffering hides progress] → Use line-buffered script (`stdbuf` / `echo` frequent) or `PYTHONUNBUFFERED`-style; flush after PROGRESS lines.
- [Parallel `jobs>1` disorder] → Count completed shards, not line order alone.
- [Event spam] → Throttle UI updates (~100–200ms) if needed.

## Migration Plan

1. Emit `PROGRESS` from compile/link loops in the script.
2. Replace `output()` with streamed spawn + emit.
3. Frontend listen + ProgressBar.
4. Smoke one Emerald Build and confirm bar moves then clears.
