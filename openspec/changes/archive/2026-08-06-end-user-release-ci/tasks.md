## 1. Package-relative path resolution

- [x] 1.1 Update launcher ROM discovery to scan package/install root `roms/`, host-binary-parent `roms/` (covers `build/roms`), then CWD/repo `roms/`
- [x] 1.2 Resolve BIOS from package/install root and host-binary parent in addition to CWD `gba_bios.bin`
- [x] 1.3 Prefer writable package/build `roms/` when Add ROM copies a dump
- [x] 1.4 Align host/CLI discovery hints with the same roots where applicable; keep developer CWD behavior working

## 2. Multi-OS packaging layout

- [x] 2.1 Extend `scripts/package_release.sh` to stage empty `roms/README.txt`, host + launcher under `bin/`, player entrypoint, scripts, docs, LICENSE
- [x] 2.2 Add Windows packaging entrypoint (PowerShell or CI steps) producing the same logical layout and a zip
- [x] 2.3 Add macOS packaging entrypoint (script or CI steps) for `macos-arm64` zip/tarball
- [x] 2.4 Add a small D7 layout check (archive must include host+launcher+`roms/`; must not include `.gba`/BIOS/covers/cart AOT)

## 3. GitHub Actions release workflow

- [x] 3.1 Add `.github/workflows/release.yml` triggered on `v*` tags (plus optional `workflow_dispatch`)
- [x] 3.2 Matrix/jobs: `ubuntu-latest`, `windows-latest`, `macos-latest` — install CMake/SDL/Node/Rust/WebView deps per OS
- [x] 3.3 Each job: submodule init, build host + `gba_recompile`, production Tauri launcher build, run packaging, upload artifact
- [x] 3.4 Final job: create/update GitHub Release for the tag and attach Linux/Windows/macOS packages via `GITHUB_TOKEN`

## 4. End-user documentation

- [x] 4.1 Document download-from-Releases flow: unzip → place USA dumps in package `roms/` → place `gba_bios.bin` at package root → run player entrypoint → Build → Play
- [x] 4.2 Document cartridge saves: in-game only, `<user-data>/saves/<sha1>.sav`, and Linux/Windows/macOS user-data roots
- [x] 4.3 Note Windows cart Build still needs Git Bash/WSL; note unsigned binary OS warnings; note macOS arm64 primary artifact
- [x] 4.4 Update README Paths / Release sections to point at CI Releases and package-relative `roms/`

## 5. Verification

- [x] 5.1 Local Linux: package script produces zip with host, launcher, empty `roms/`; launcher lists a dump placed in package `roms/` without special CWD
- [x] 5.2 Developer path: dump in `build/roms` is discovered when playing via that host
- [x] 5.3 Confirm save path docs match `cartridge_save_path` behavior; existing save tests still pass
- [x] 5.4 Dry-run workflow (`workflow_dispatch` or act) on at least Linux; confirm D7 check fails if a `.gba` is accidentally staged
