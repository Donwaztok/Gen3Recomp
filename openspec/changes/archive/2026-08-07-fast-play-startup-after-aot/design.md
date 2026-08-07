## Context

See proposal.md — Why. User report: Play stays ~1–2 minutes **every** launch after Emerald cart AOT is built, not only the first time.

Current path (Linux reference):
- Play spawns a new `gen3recomp` (no cart rebuild).
- `try_activate_cart_artifact` does `dlopen(libcart.so, RTLD_NOW)` (~187 MiB).
- gba-recomp `warm_load_cache()` eagerly walks the heal cache and `dlopen`s every `*.dll` shard (`overlay_loader.cpp`) — often 10k+ files under `~/.local/share/gen3recomp/recomp_cache/<sha1>/`.

Constraints: D7 (no shipping artifacts), provider seam only, title-agnostic runtime, keep Win/macOS from regressing.

## Goals / Non-Goals

**Goals:**
- Measure where repeated-Play time goes
- Default path: no full-cache eager warm before guest runs
- Cheaper cart artifact activation for repeated Play
- Spec + launcher copy match “Build once, then open quickly”
- Target: typically **&lt; ~5s** to first guest frames when AOT + heal cache are warm (SSD Linux)

**Non-Goals:**
- Instant one-time cart Build
- Full Emerald `[[code_copy]]` IWRAM elimination in this change (follow-up if cold empty-cache Play remains the complaint)
- Always-on long-lived host daemon as the only fix

## Decisions

### D1 — Primary bottleneck for “every time”
- **Choice:** Treat eager heal-cache warm-load + eager cart `RTLD_NOW` as the default repeated-Play cost; fix those first.
- **Why:** Matches “every Play” report; rebuild is already ruled out on Play.
- **Alt:** Assume only first-boot gcc heal — rejected for this user’s clarification.

### D2 — Default warm policy = on-demand
- **Choice:** Default player path skips full `warm_load_cache()` (or equivalent); load overlay shards when a PC misses and a valid cache file exists. Keep optional eager warm via env (e.g. `GBARECOMP_HEAL_EAGER_WARM=1`) for diagnostics.
- **Why:** Smallest change with largest win; preserves correctness of disk cache.
- **Alt:** Parallel warm of all shards — still pays N×dlopen before frames; weaker.
- **Note:** Prefer adapter/host policy or minimal pin patch in `third_party/gbarecomp` with clear upstream-oriented comments.

### D3 — Cart `dlopen` flags
- **Choice:** Switch cart activation toward `RTLD_LAZY` (or measure and keep `NOW` only if LAZY is unsafe for the artifact ABI). Log activation milliseconds.
- **Why:** Eager reloc of ~187 MiB every process is a repeated tax.
- **Alt:** Keep process alive across Play — larger architecture; defer.

### D4 — Success budget
- **Choice:** Acceptance = warm repeated Play typically under ~5s to first guest frames on reference Linux SSD; log phases so failures are diagnosable.
- **Why:** “Instant” as player language; measurable engineering bar.

### D5 — Cold empty heal cache
- **Choice:** Document as distinct; out of primary acceptance for this change. Optional follow-up: `code_copy` for IWRAM.
- **Why:** User’s pain is every-time cost after they already played/compiled.

## Risks / Trade-offs

- [On-demand first hit hitch mid-boot] → Accept small hitches vs minute pre-stall; optionally prefetch a tiny boot PC set later.
- [RTLD_LAZY surprises unresolved symbols] → Measure; fall back to NOW if broken; keep tests/smoke.
- [Vendoring gba-recomp drift] → Prefer thin hook/flag; document pin delta.
- [Windows LoadLibrary semantics differ] → Linux reference first; ensure Win path does not eagerly walk 10k files if applicable.

## Migration Plan

1. Add phase timers (cart activate, warm_loaded count/ms, first frame).
2. Default-disable full eager warm; verify second Play with existing cache.
3. Adjust cart `dlopen` flags; re-measure.
4. Update launcher/docs wording; smoke Emerald Play twice after AOT ready.
