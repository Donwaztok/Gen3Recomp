## 1. Phase 1 — Official local AOT path (Emerald)

- [x] 1.1 Commit/stabilize gitignored-aware CMake hook for optional `generated/rom/` shards + stub fallback; ensure clean CI builds without corpus
- [x] 1.2 Finish `scripts/recompile_user_rom.sh` + `scripts/scan_rom_symbols.py` defaults for Emerald USA; clear stale shards before regenerate
- [x] 1.3 Keep `data/emerald_usa.toml` with `static_resume_all = false`, AOT scan range, and documented shard count
- [x] 1.4 Update README and `docs/manual-boot.md` so BIOS+cart AOT is the primary path; demote `--prepare` to optional/diagnostic
- [x] 1.5 Adjust `--help` text so `--prepare` is not presented as the full-speed solution
- [x] 1.6 Verify Emerald launch logs `static_cart=true` (or equivalent) with linked corpus; document expected IWRAM heal leftovers

## 2. Phase 2 — Cart artifact in user data

- [x] 2.1 Choose artifact shape against gba-recomp pin (dedicated shared library vs reuse heal-cache layout); document in adapter comments
- [x] 2.2 Add script or provider step that builds the cart artifact into `~/.local/share/gen3recomp/…/<sha1>/` with an abi/version path segment
- [x] 2.3 Teach gba adapter prepare to prefer: user-data artifact → linked `generated/rom` → heal-only empty dispatch
- [x] 2.4 Ensure artifact load stays inside `src/recomp/gba/`; no app/runtime plugin scan
- [x] 2.5 Add a unit/integration test or guarded local test for “missing artifact messaging” and “artifact path includes SHA-1”
- [x] 2.6 Document player flow: one-time artifact build, then normal `--rom` / `--bios` launch without relinking the host

## 3. Phase 3 — IWRAM / AOT quality

- [x] 3.1 Collect Emerald IWRAM heal PCs from a static-AOT session miss fragment / logs
- [x] 3.2 Add `[[code_copy]]` (and any needed TOML hints) to `data/emerald_usa.toml` for known Gen3 overlays
- [x] 3.3 Confirm RAM overlay heal remains enabled by default; ROM static coverage does not disable it
- [x] 3.4 Re-run cart recompile after TOML changes; spot-check boot hitch reduction past logos into title/overworld

## 4. Phase 4 — Ruby and Sapphire pipeline

- [x] 4.1 Add `data/ruby_usa.toml` and `data/sapphire_usa.toml` with catalog-matching SHA-1 and AOT scan bounds
- [x] 4.2 Extend `recompile_user_rom.sh` (or thin wrappers) to select config from identified title / env / arg
- [x] 4.3 Document Ruby/Sapphire AOT + artifact steps in README / manual-boot (dumps optional on CI)
- [x] 4.4 Manual or guarded boot checklist entries for Ruby/Sapphire title screen with static AOT when dumps are present

## 5. Validation and cleanup

- [x] 5.1 Run `ctest --test-dir build --output-on-failure` on clean (no corpus) and with Emerald corpus if available
- [x] 5.2 Confirm no ROM/BIOS/generated cart sources are staged for commit; submodule dirtiness from BIOS recomp stays local
- [x] 5.3 Archive-ready: specs/design/tasks consistent; product docs no longer sell `--prepare` as the Gen3 speed solution
