## Why

End users will not run CMake or shell scripts. After local cart AOT exists as a product path (D7), the missing piece is a friendly host: open the exe, pick a catalogued ROM (or drop it in `roms/`), let the app build the cart artifact when needed, then play—plus a simple mod list that stays under user control without shipping Nintendo data.

## What Changes

- Default launch with **no `--rom`** opens a small **launcher window** (ROM list, BIOS status, Build / Play, mods panel)—not a silent first-file autoplay.
- Discover catalogued dumps from a local **`roms/`** folder (beside the executable or CWD) and allow **Add ROM…** via a native file picker; reject unknown SHA-1s with a clear message.
- **AOT gate:** Play stays disabled until a valid cart artifact (and BIOS AOT/policy) is ready for the selected dump. The UI explains that a one-time recompile is required, shows progress/errors, then unlocks Play.
- **BREAKING (player path):** Cart AOT must become **activable at runtime** for the shipped host binary (no “reconfigure CMake and relink gen3recomp” for end users). Dev link of `generated/rom/` may remain for contributors.
- **Mod manager (launcher):** list packages under a user `mods/` directory, enable/disable, persist selection per dump or globally; refuse packages that embed ROM/BIOS bytes.
- **In-game mod control (stretch in this change):** if feasible behind the gba adapter without a plugin ABI (D6), expose a pause/overlay toggle for enablement that mirrors launcher state; otherwise ship launcher-only toggles and document in-game as follow-up.
- Keep **CLI** `--rom` / `--bios` for automation and power users.
- Keep **D7**: Releases still do not ship ROM-derived cart code; recompile happens on the user’s machine.
- Update product decision **D3**: CLI-first remains supported; the **default player experience** becomes the launcher UI (supersedes “no launcher shell” for end users).

## Capabilities

### New Capabilities
- `launcher-ui`: Player-facing window to select ROM/BIOS, run the AOT gate, and start a session without CLI.
- `mod-manager`: Discover, list, and enable/disable user-supplied mod packages without distributing Nintendo assets.

### Modified Capabilities
- `launcher`: Default startup without `--rom` opens the launcher UI (instead of usage-only or a single opaque picker-then-boot); CLI path unchanged when `--rom` is provided.
- `recompiler-provider`: Prepare/play path must consume a ready user-data cart artifact (or build it) without requiring a host relink for the player UX; still isolate gba-recomp to the adapter.
- `native-boot`: Boot from the launcher after a successful AOT gate uses the same one-host path for catalogued titles.

## Impact

- Modules: `app` (orchestration + launcher UI), `platform` (window/dialogs), possibly thin UI helpers still under `app` (no new top-level module unless design requires updating architecture.md).
- Depends on cart artifact work from `static-cart-aot-pipeline` (archive or merge first if still open).
- Legal: D7/D9 unchanged; mods must not ship ROM-derived blobs.
- Non-goals: Thunderstore client, shipping prebuilt `libcart.so`, FireRed/LeafGreen catalog expansion, full Zelda-style binary mod ABI in v1, Qt/Electron shell, cover-art browsers.

Assumptions recorded for design: SDL3-based simple UI (not Qt); mods v1 = enable flags + package manifest applied at prepare; in-game toggle is best-effort in the same change.
