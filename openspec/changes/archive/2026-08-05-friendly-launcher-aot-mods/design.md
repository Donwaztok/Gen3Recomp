## Context

See `proposal.md`. Cart AOT and `libcart.so` under user data come from `static-cart-aot-pipeline` (still open/complete but player path today often needs CMake `-DGEN3RECOMP_CART_ARTIFACT` relink). D3 was CLI-first; this change makes a small launcher the default player path while keeping CLI. D6 forbids plugin recompilers; mods are data/hook packages into the gba adapter. D7 unchanged: no shipping ROM-derived cart code.

## Goals / Non-Goals

**Goals:**
- Double-click → launcher → select ROM → one-time AOT build → Play.
- Scan `roms/`, add via picker, never autoplay first file.
- List/enable mods from `mods/`; persist; reject ROM/BIOS payloads.
- Runtime-activate cart artifact on a stock Release binary (unblock Play without CMake).
- Keep `--rom` CLI for scripts.

**Non-Goals:**
- Qt/Electron; Thunderstore client; shipping `libcart.so` in GitHub Releases.
- Full Zelda binary mod ABI / arbitrary native plugins in v1.
- FRLG catalog; cover art grids; multiplayer.

## Decisions

### D-UI-1 — Supersede D3 for default player path
Default with no `--rom` → launcher UI. CLI remains first-class when flags are passed. Update `openspec/product/decisions.md` D3 accordingly during apply.

**Rejected:** CLI-only forever; forcing `--rom` for all users.

### D-UI-2 — SDL3 + lightweight immediate UI
Implement the launcher with SDL3 (already required) plus a small immediate-mode UI (e.g. Dear ImGui) or an equivalently thin custom SDL UI—no Qt.

**Rejected:** Native-only dialogs without a persistent ROM list (too weak for multi-dump + AOT status + mods).

### D-UI-3 — Runtime cart activation (required for UX)
Stock `gen3recomp` keeps empty `kDispatchTable` stub. Ready `libcart.so` is activated inside `src/recomp/gba/` at prepare/Play (dlopen + symbol registration or equivalent upstream-safe mechanism). If upstream cannot register a table without a patch, apply a minimal pin-local change behind the adapter and document it.

**Rejected:** Asking end users to run CMake after Build; shipping multiple per-title executables.

### D-UI-4 — AOT build invoked by the host
Build action shells out to or embeds the same pipeline as `build_cart_artifact.sh` (generate + compile `.so` into user-data), with UI progress and logs. Requires a working host compiler on the player machine for v1 (document clearly).

**Rejected:** Silent network download of prebuilt cart objects (violates D7 / trust).

### D-UI-5 — Mods v1 = manifest packages + enable flags
Layout: `mods/<id>/mod.toml` (name, version, optional hooks/patches declared as data). Enablement stored in user config. Adapter applies only supported hook kinds; unknown kinds fail closed.

In-game toggle: attempt a simple pause-menu or hotkey that writes the same config; if upstream loop ownership blocks it, ship launcher-only and note follow-up.

**Rejected:** dlopen of arbitrary mod `.so` as alternate recompilers (conflicts with D6).

### Dependency direction
```
app (launcher UI + mod list)
  → game (catalog identify)
  → recomp provider (readiness, build trigger, prepare + mods)
       → gba adapter (runtime cart load, mod apply)
  → platform (SDL window / file dialogs)
```

## Risks / Trade-offs

- **[Risk] Players lack a C++ toolchain** → Mitigation: document packages; optional future “portable toolchain” bundle; clear UI error.
- **[Risk] dlopen cart vs link-time `kDispatchTable`** → Mitigation: adapter-owned registration or minimal upstream patch; prototype early in tasks.
- **[Risk] Long first build (minutes)** → Mitigation: progress UI; resume/cached objects; never block without messaging.
- **[Risk] Mod scope creep** → Mitigation: v1 enable/disable + manifest only; rich scripting later.
- **[Trade-off] ImGui adds a dependency** → Accept for UX; keep behind app, not runtime.

## Migration Plan

1. Archive or land `static-cart-aot-pipeline` artifacts into main specs if not already.
2. Implement runtime cart load + readiness API (blocker for UI Play).
3. Ship launcher UI + `roms/` discovery + AOT gate calling build pipeline.
4. Add mod manager list/enable + prepare plumbing; in-game toggle if feasible.
5. Update D3, README, release notes: zip = host + tools; user brings ROM/BIOS; first Build is local.

Rollback: `--rom` CLI and heal-only/stub path remain.

## Open Questions

- Exact ImGui (or alternative) packaging via FetchContent vs system package—**resolved at apply:** SDL3 `SDL_RenderDebugText` + native file dialogs (no ImGui dependency).
- Whether in-game mod toggle fits the first apply slice—**resolved:** deferred. Upstream `run_game` owns the window/loop; launcher-only enablement for this change (documented in README / manual-boot).

## Apply notes (1.3 spike)

Upstream `kDispatchTable` is link-time. Minimal pin-local API added:

- `runtime_set_cart_dispatch(const void* table, unsigned len)` in `third_party/gbarecomp/src/armv4t/runtime_arm.{h,cpp}`
- Host `try_activate_cart_artifact` dlopens user-data `libcart.so`, `dlsym`s `kDispatchTable` / `kDispatchTableLen`, registers override, keeps the handle loaded.
