## Why

Cold self-heal and `--prepare` leave first visits to guest code at a few FPS; that is not a playable Gen3 host. Zelda64Recomp proves the right product model is full-cart static AOT. gen3recomp cannot ship ROM-derived C++ (D7), so preparation must happen on the user’s machine, then stay fast across the whole dump—not just the title menu.

## What Changes

- Make **local full-cart static AOT** the official play path for catalogued USA dumps (scripts, CMake optional link of `generated/rom/`, docs). Demote `--prepare` to an optional diagnostic tool (or remove it from user-facing docs).
- Add a **durable per-dump cart artifact** under user data (SHA-1 keyed) so a rebuilt host can load native cart code without recompiling `gen3recomp` every time.
- Improve **Emerald AOT quality**: IWRAM overlay coverage (`[[code_copy]]` / heal policy) so IRQ/m4a/`AgbMain` copies are not a cold hitch wall; tighten discovery config without shipping pret C.
- Extend the **same AOT + artifact pipeline** to catalogued Ruby USA and Sapphire USA (TOML + scripts + Game Definitions already in catalog).
- Keep **CLI-first launcher** (D3): `--rom` / `--bios`, optional native picker when `--rom` is omitted; if multiple catalogued dumps are offered in one flow, present an explicit choice—never silently pick the first file in a folder. Default BIOS discovery for `./gba_bios.bin` next to CWD/exe remains allowed.

## Capabilities

### New Capabilities
- `static-cart-aot`: User-local ahead-of-time recompilation of a catalogued GBA cart into native code the host can execute at full speed.
- `cart-artifact`: Durable, SHA-1-keyed native cart artifact in user data that prepare can reuse across host rebuilds without committing ROM-derived sources.

### Modified Capabilities
- `launcher`: Document and require AOT-oriented startup messaging; treat `--prepare` as non-primary; clarify BIOS default path and multi-dump selection rules.
- `recompiler-provider`: Prefer a static cart artifact (or developer-linked corpus) over empty-dispatch cold self-heal for catalogued titles; still forbid version-controlled ROM-derived sources.
- `native-boot`: MVP boot success includes playable speed through title into early gameplay when static AOT is present, not only “title screen eventually appears after minutes of heal”.

## Impact

- Modules: `recomp` (provider/backend, CMake glob or artifact load), `app`/`launcher` CLI and docs, `game` catalog TOMLs for Ruby/Sapphire AOT configs, scripts under `scripts/`.
- Upstream: continues to use gba-recomp `gba_recompile` + runtime; no new modules; no plugin ABI (D6)—artifact load stays inside the gba adapter.
- Legal: `generated/rom/`, BIOS recomp under the submodule, and user-data artifacts remain gitignored / local-only (D7, D9).
- Non-goals: FireRed/LeafGreen, shipping prebuilt cart `.so` in releases, ImGui/Qt launcher shell, replacing gba-recomp with pret pokeemerald-recomp as the execution engine, Vulkan/mods/save-states.
