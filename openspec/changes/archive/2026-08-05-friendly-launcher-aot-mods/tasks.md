## 1. Prerequisites and decisions

- [x] 1.1 Archive or sync `static-cart-aot-pipeline` into main specs so cart-artifact / AOT paths are the baseline
- [x] 1.2 Update `openspec/product/decisions.md` D3: default player path is launcher UI; CLI remains supported
- [x] 1.3 Spike runtime cart activation (`dlopen` / register dispatch) inside `src/recomp/gba/`; record result in design if upstream patch is required

## 2. Runtime cart artifact for players

- [x] 2.1 Implement readiness query: artifact path exists + abi tag matches for a SHA-1
- [x] 2.2 Implement runtime load/activation of `libcart.so` on prepare/Play for stock stub-linked host
- [x] 2.3 Wire Build pipeline invocation (reuse `build_cart_artifact.sh` logic) with progress/log callbacks for UI
- [x] 2.4 Ensure missing toolchain / build failure surfaces a clear error string to the host

## 3. Launcher UI

- [x] 3.1 Add SDL3 launcher window path when `--rom` is omitted; keep CLI path when `--rom` is set
- [x] 3.2 Scan `roms/` (exe dir + CWD), identify via catalog SHA-1, list selectable titles
- [x] 3.3 Add ROM… via native file picker; never autoplay first file
- [x] 3.4 Show BIOS status; block Play until valid BIOS is present
- [x] 3.5 AOT gate UI: Build disabled/enabled Play, status text, progress during recompile
- [x] 3.6 Play starts provider session for the selected dump

## 4. Mod manager

- [x] 4.1 Define `mods/<id>/mod.toml` manifest schema (name, version, optional hooks) and discovery roots
- [x] 4.2 Launcher panel: list mods, enable/disable, persist config under user data
- [x] 4.3 Reject packages that embed `.gba` / BIOS payloads
- [x] 4.4 Pass enabled mod set into prepare; adapter applies supported hooks or fails closed
- [x] 4.5 Spike in-game toggle against upstream owned loop; implement or document launcher-only for this change

## 5. Docs and validation

- [x] 5.1 README / manual-boot: player flow (roms/, Build once, Play); Release zip contents vs user dumps
- [x] 5.2 `--help` mentions bare launch opens UI; CLI `--rom` still documented
- [x] 5.3 Automated tests for: bare launch enters UI mode (or testable seam), roms discovery filtering, mod enable persistence, artifact readiness API
- [x] 5.4 Manual checklist: Emerald from launcher after Build; multi-ROM selection; unknown dump rejected
