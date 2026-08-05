## Context

Host loop and validation exist. This is the integration wound. Read the **pinned** gba-recomp revision’s architecture before coding; upstream moves quickly.

## Goals / Non-Goals

**Goals:**
- Pin upstream.
- Adapter implements prepare + session backend.
- Runtime uses gba backend after successful prepare.
- Attribute gba-recomp in third-party docs; root license is already PolyForm NC (D11).
- Tests: seam tests with fakes; one guarded integration test if a local ROM path is provided via env var (never in CI by default).

**Non-Goals:**
- Perfect static coverage of all code paths.
- Exposing upstream launcher, mods, widescreen, save states.
- Reimplementing PPU/APU.
- FireRed/LeafGreen.

## Decisions

- **Consume upstream as a library/runtime**, not by forking EmeraldRecomp into this repo.
- **User-local execution path:** interpreter + JIT healing + disk cache under user data, keyed by ROM SHA-1. Do not commit generated C++.
- **If upstream host loop fights our loop:** wrap upstream’s run function behind `session.start/stop` even if `step` is coarse (run-until-quit). Prefer a steppable API if it exists; otherwise one “run blocking with callbacks” adapter is acceptable if it keeps the host simpler. Record the choice when the pin is known.
- **Symbols/metadata:** store only what the adapter needs, as data next to the catalog (addresses/names), sourced like upstream game repos do from pret maps — never pret C source.
- **Null backend remains** for tests and as fallback only if explicitly requested (debug). Default path after M07 is gba provider.

## Risks / Trade-offs

- [Upstream is really an application framework, not a library] → Adapter may need a thicker wrap. Still isolate it. If linking is impractical, spawning upstream tools is a last resort and must stay inside the adapter.
- [License surprise] → Root LICENSE is already PolyForm NC; attribute upstream next to the pin.
- [API mismatch with our step loop] → Update this design; do not leak upstream into runtime.

## Migration Plan

Add `third_party/README.md` with pin hash and upgrade steps.

## Open Questions

- Resolved for M07: upstream entry point is blocking `gbarecomp::run_game` with its own SDL2 host. M07 pins revision `2952aff` and lands the provider/session adapter without linking `gbarecomp_runtime`. M08 will wrap upstream execution or a steppable subset behind the same adapter.
