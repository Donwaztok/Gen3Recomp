## Why

The project already has a Linux-only packaging script and a working player pair (host + Tauri launcher), but there is no automated GitHub Actions release path and the install layout still assumes developer CWD/`roms/` rather than a self-contained player package. End users need downloadable Linux, Windows, and macOS artifacts with clear docs for ROMs, BIOS, and where cartridge saves live.

## What Changes

- Add a GitHub Actions workflow that builds the player package (CMake host + Tauri launcher + scripts + docs) and publishes artifacts to GitHub Releases on version tags.
- Extend packaging beyond the current Linux-only `scripts/package_release.sh` so Linux, Windows, and macOS each get a D7-safe player archive (or platform-native bundle) that includes both `gen3recomp` and `gen3recomp-launcher`.
- Adjust ROM (and BIOS) discovery so a packaged/build tree works: empty `roms/` lives inside the package/build directory beside the player binaries, and the launcher scans that location without requiring a repo checkout as CWD.
- Document cartridge save behavior and OS-specific paths for end users (in-game saves only; no save states in MVP).
- Keep Releases free of ROM dumps, BIOS images, covers, and cart AOT objects (D7 / legal).

### Non-goals

- Shipping ROMs, BIOS, cover art, or prebuilt cart AOT in Releases.
- Save states, cloud sync, or migrating third-party emulator `.sav` formats.
- Native Windows cart AOT without bash/WSL (document current limitation only).
- App Store / Microsoft Store / Flatpak / Homebrew distribution channels.
- Replacing the existing developer CMake/Tauri workflows.

## Capabilities

### New Capabilities

- `player-release`: GitHub Actions–driven multi-OS player packages published to GitHub Releases, including package layout (host + launcher + empty `roms/` + docs) and D7 contents rules.

### Modified Capabilities

- `build-system`: Automated release packaging becomes a first-class, documented path; multi-OS player artifacts are produced by CI (not only a manual Linux zip script).
- `launcher-ui`: ROM discovery MUST treat the package/build `roms/` directory (beside the player install root / host binary) as a primary scan root so end-user layouts work without a repo CWD.
- `cartridge-save`: End-user documentation MUST describe cartridge save locations per OS and that MVP supports in-game saves only.

## Impact

- New `.github/workflows/` release workflow; likely evolution of `scripts/package_release.sh` (and Win/mac equivalents or a shared packaging entrypoint).
- Launcher path resolution (`launcher/src-tauri/src/paths.rs`) and possibly host BIOS/ROM hints aligned to package-relative `roms/` / `gba_bios.bin`.
- README / `docs/manual-boot.md` (or a dedicated player guide) updated for download → place ROM/BIOS → Build → Play → where saves live.
- CI needs Node, Rust, CMake, SDL, and platform WebView tooling on Linux/Windows/macOS runners; release secrets limited to default `GITHUB_TOKEN` unless signing is added later.
