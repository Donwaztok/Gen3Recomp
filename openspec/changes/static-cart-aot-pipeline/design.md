## Context

See `proposal.md` for motivation. Today the host can link a gitignored Emerald corpus under `generated/rom/` (64 shards, ~138k dispatch entries) or fall back to empty cart dispatch + runtime self-heal. `--prepare` warms only a frame window. BIOS AOT already lives under the gba-recomp submodule as local-only output. D3 keeps the launcher CLI-first; D6 forbids a plugin provider ABI; D7 forbids shipping ROM-derived C++.

## Goals / Non-Goals

**Goals:**
- Make local full-cart AOT the official Emerald path, then the same for Ruby/Sapphire.
- Introduce a SHA-1-keyed cart artifact in user data so host rebuilds do not require recompiling ~800 MiB of generated C++ into `gen3recomp`.
- Cover IWRAM overlays via config/`code_copy` and existing RAM overlay heal—not by pretending ROM AOT includes RAM PCs.
- Keep selection explicit (CLI `--rom` or one picker); never autoplay the first `.gba` in a folder.

**Non-Goals:**
- Shipping prebuilt cart libraries in GitHub Releases.
- ImGui/Qt multi-page launcher.
- Replacing gba-recomp with pret pokeemerald-recomp as the engine.
- FireRed/LeafGreen, mods, save states, Vulkan.

## Decisions

### D-AOT-1 — Two consumption modes for cart AOT
1. **Dev link mode (phase 1):** CMake optionally compiles `generated/rom/*.cpp` into `gen3recomp_recomp` when present (already sketched). Fastest path to validate Emerald.
2. **Artifact mode (phase 2):** `gba_recompile` + host compiler produce a shared library (or gba-recomp-compatible cache shape) under `~/.local/share/gen3recomp/…/<sha1>/`. The gba adapter loads it at prepare time.

**Why both:** Link mode unblocks play now. Artifact mode is the Zelda-like UX without violating D7 (player builds the artifact; we ship the host).

**Rejected:** Only `--prepare` forever; shipping `.so` in the release zip.

### D-AOT-2 — Artifact loading stays inside `src/recomp/gba/`
`dlopen` (or upstream cache load) of cart code is adapter-private. App/runtime only see prepare success/failure. This preserves D6 (“no plugin providers”) while allowing one controlled cart blob.

**Rejected:** Generic plugin directory for alternate recompilers.

### D-AOT-3 — Keep `static_resume_all = false` for carts
Full instruction-boundary resume explodes dispatch size (~1M+ aliases). IRQ mid-function resume continues to use runtime self-heal / present-in-place. BIOS may keep `static_resume_all` as upstream does.

**Rejected:** Enabling `static_resume_all` on Emerald for “perfect” AOT at the cost of unbuildable corpora.

### D-AOT-4 — IWRAM via heal + TOML `code_copy`, not pret C
Use gba-recomp `[[code_copy]]` / overlay heal for known Gen3 RAM copies. Pret symbols may seed discovery TOMLs later but never become the shipped execution model.

**Rejected:** Switching the product to pokeemerald-recomp.

### D-AOT-5 — Per-title AOT TOML as Game Definition adjacent data
`data/emerald_usa.toml` (and ruby/sapphire counterparts) feed `gba_recompile`. Identity SHA-1 must match the catalog. Title logic stays out of runtime.

### D-AOT-6 — Launcher stays CLI-first; selection is explicit
Default BIOS: `./gba_bios.bin` when `--bios` omitted (already partially true). ROM: `--rom` or single native picker. Helpers that scan a folder MUST require a single match or prompt—never silent first-file.

**Rejected:** Emulator-style ROM browser as MVP scope (can follow later without changing prepare).

### Dependency direction
```
app → game (catalog) → recomp provider
                    ↘ gba adapter → gba-recomp runtime
                                    ↘ optional: linked generated/rom OR loaded cart artifact
platform (picker only) stays game-agnostic
```

## Risks / Trade-offs

- **[Risk] Huge compile times / RAM for 64 shards** → Mitigation: artifact mode builds once per dump; compile flags `-w` / no var-tracking; document `ccache`; keep shard count high.
- **[Risk] False-positive prologue seeds bloat / wrong code** → Mitigation: prefer walk+config; treat self-heal miss TOML fragments as review input; tighten configs per title over time.
- **[Risk] Artifact ABI skew after gba-recomp pin bump** → Mitigation: include abi/version segment in artifact path (upstream already versions heal cache); invalidate on pin change.
- **[Risk] Users confuse `--prepare` with full AOT** → Mitigation: demote in docs/help; primary scripts are `recompile_user_bios.sh` / `recompile_user_rom.sh`.
- **[Trade-off] Dev link mode duplicates work with artifact mode** → Accept temporarily; artifact mode becomes default player path, link mode remains contributor shortcut.

## Migration Plan

1. Land docs + scripts + Emerald TOML + CMake hook (phase 1); keep stub path for clean CI.
2. Add artifact build/load in the gba adapter; prefer artifact, then linked corpus, then heal-only.
3. Add Emerald IWRAM `code_copy` / overlay config from observed heal PCs.
4. Add `data/ruby_usa.toml` / `data/sapphire_usa.toml` and verify catalog SHA-1s; same scripts.
5. Archive change; update product roadmap note that full-speed MVP means AOT artifact, not frame warm-up.

Rollback: remove artifact load and fall back to linked corpus or heal-only; CMake stub path already exists.

## Open Questions

- Whether `--prepare` remains in the CLI long-term or is removed in a follow-up (kept as diagnostic for now).

## Phase-2 decision (resolved)

Cart artifact format: dedicated `libcart.so` under
`~/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/` exporting
`kDispatchTable` / `kDispatchTableLen`. Host links it via
`-DGEN3RECOMP_CART_ARTIFACT=...`. Heal-cache DLLs remain IWRAM-only.
Upstream does not expose a cart-dispatch registration API; link-time
symbols are the workable shape without forking `runtime_arm.cpp`.
