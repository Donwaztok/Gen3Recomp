# Player guide

Download a player package from [GitHub Releases](../../releases) for your OS (Linux, Windows, or macOS arm64). Packages are built by GitHub Actions on version tags (`v*`).

This project does **not** ship Nintendo ROMs, GBA BIOS images, cover art, or prebuilt cart objects. You must supply your own legally obtained dumps.

## Install and play

1. Download and unzip the package for your platform (`gen3recomp-<ver>-linux-x64.zip`, `…-windows-x64.zip`, or `…-macos-arm64.zip`).
2. Place USA Ruby, Sapphire, or Emerald `.gba` dumps in the package’s `roms/` folder.
3. Place `gba_bios.bin` in the **package root** (next to `gen3recomp-player` / `gen3recomp-player.bat`).
4. Run the player entrypoint:
   - Linux / macOS: `./gen3recomp-player`
   - Windows: `gen3recomp-player.bat`
5. In the launcher: select a cover → **Build** once (cart AOT) → **Play**.

Developer builds can also use `build/roms/` next to `build/gen3recomp`.

## Saves

MVP supports **in-game cartridge saves only** (no save-state slots).

| Kind | Location |
|------|----------|
| Save file | `<user-data>/saves/<sha1>.sav` |

| OS | User-data default |
|----|-------------------|
| Linux | `~/.local/share/gen3recomp` (`XDG_DATA_HOME` honored) |
| Windows | `%APPDATA%\gen3recomp` |
| macOS | `~/Library/Application Support/gen3recomp` |

Saves are keyed by ROM SHA-1 so titles do not overwrite each other. They live in user data (not inside the zip) so re-downloading a Release does not wipe progress.

## Platform notes

- **Windows:** Cart **Build** still shells to `scripts/build_cart_artifact.sh` and needs Git Bash or WSL. Play works once a cart artifact exists. SmartScreen may warn on unsigned binaries — allow the app if you trust the Release source.
- **macOS:** Primary CI artifact is **Apple Silicon (arm64)**. Gatekeeper may block unsigned apps — right-click → Open the first time.
- **Linux:** On Wayland + NVIDIA, the player wrapper sets `GDK_BACKEND=x11` for WebKit stability.

## Package layout

```
gen3recomp-<ver>-<os>-<arch>/
  gen3recomp-player(.bat)     # start here
  bin/gen3recomp[.exe]
  bin/gen3recomp-launcher[.exe]
  roms/README.txt             # put .gba dumps here
  scripts/                    # cart AOT helpers
  docs/
  LICENSE
```

## Maintainer: publishing a Release

1. Push a tag matching `v*` (example: `git tag v0.1.0 && git push origin v0.1.0`).
2. The `Release` workflow builds Linux, Windows, and macOS packages and attaches them to the GitHub Release.
3. Optional dry-run: Actions → Release → Run workflow (`workflow_dispatch`).

Local packaging:

```sh
./scripts/package_release.sh                          # Linux (or macOS with GEN3RECOMP_PLATFORM=macos-arm64)
GEN3RECOMP_PLATFORM=macos-arm64 ./scripts/package_release.sh
powershell -File scripts/package_release.ps1          # Windows
./scripts/verify_release_layout.sh dist/release/gen3recomp-*-linux-x64
```
