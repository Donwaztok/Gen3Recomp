## Why

After cart AOT is ready, every Play still takes on the order of a minute or more before the game is usable — not only the first launch. Players reasonably expect “Build once, then open quickly”; today each new host process still pays a heavy startup tax (eager heal-cache warm-load of thousands of overlay DLLs plus eager `dlopen` of a ~hundreds-of-MiB `libcart.so`).

## What Changes

- Make **repeated Play** (cart artifact already built; heal cache already populated) reach early boot / title path in **seconds**, not ~1–2 minutes, on a typical warm SSD Linux machine.
- Stop treating “full-speed cart AOT” as if it implied fast process startup; add observable startup-time expectations when AOT + warm heal cache are present.
- Change overlay heal warm policy so startup does **not** eagerly `dlopen` every cached overlay shard before the guest runs (load on demand / miss, or an equivalent cheap path).
- Reduce cart-artifact activation cost per Play (cheaper bind flags and/or other load strategy) so remapping `libcart.so` is not a multi-tens-of-seconds gate every session.
- Add phase timing logs so maintainers can see whether time is spent in cart `dlopen`, heal warm-load, or guest boot.
- Clarify launcher/docs: Build finishes cart AOT; Play startup cost is separate and must stay small once caches exist.

### Non-goals

- Making the one-time cart AOT **Build** itself instant
- Shipping cart `.so` / heal caches in git or Releases (D7)
- Eliminating IWRAM heal forever via full Emerald `code_copy` in this change (may be a follow-up if cold first Play after empty heal cache remains slow)
- Keeping a long-lived host process as the only solution (may be an optional later optimization)
- FireRed/LeafGreen, Windows-only loaders as the primary target (Linux reference first; keep Win/macOS from regressing)

### Assumption

“Near-instant” here means **typically under ~5 seconds** from Play/host start to first guest frames / BIOS intro when cart AOT and a populated heal cache already exist — not sub-100ms absolute. Exact budget may be tuned after measurement in design/apply.

## Capabilities

### New Capabilities

_(none)_

### Modified Capabilities

- `native-boot`: When cart AOT is ready and heal cache is warm, repeated launches MUST reach early boot without minute-scale startup delay.
- `cart-artifact`: Activating an existing cart artifact MUST NOT impose a minute-scale cost every Play; load strategy MUST prefer a fast path for repeated sessions.
- `recompiler-provider`: Overlay/heal cache policy MUST NOT require eagerly loading every cached shard before guest execution when a cheaper on-demand path works.
- `launcher-ui`: Status copy MUST distinguish Build (cart AOT) from Play startup; after AOT is ready, Play MUST NOT imply another multi-minute wait when caches are warm.

## Impact

- Host/provider: `cart_artifact.cpp` (`dlopen` flags), gba-recomp `overlay_loader.cpp` warm-load path (vendored pin / adapter policy), session prepare logging
- Possibly env flags to keep eager warm for diagnostics
- Launcher copy in `launcher/src/App.tsx` / docs (`docs/manual-boot.md`, README) if wording still implies Build alone ends all waiting
- Measurement first: timestamp cart activate vs `warm_loaded` vs first frame

Milestone: post-AOT player UX — repeated Play startup performance.
