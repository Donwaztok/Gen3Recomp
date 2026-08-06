## Context

See proposal.md for motivation. Today `scripts/package_release.sh` builds Linux host + Tauri launcher into `dist/release/…-linux-x64.zip` but creates no `roms/` placeholder and is not wired to CI. The Tauri launcher’s `roms_dirs` scans only CWD and repo-root `roms/`; BIOS is CWD-only (`gba_bios.bin`). Cartridge saves already work at `<user-data>/saves/<sha1>.sav` via `src/core/user_data.cpp` (D13) but end-user docs need a clear multi-OS callout.

Constraints: D7/legal — never ship ROM/BIOS/covers/cart AOT. Player pair remains `gen3recomp` + `gen3recomp-launcher` (D3/D10). Prefer extending the existing packaging script over inventing a second packaging system.

## Goals / Non-Goals

**Goals:**

- One tag-triggered GitHub Actions workflow that builds and uploads Linux, Windows, and macOS player packages.
- Package layout that is runnable after unzip: host + launcher + entrypoint + empty `roms/` + docs.
- Path resolution so package-relative and `build/roms` layouts work without a repo CWD.
- Document saves accurately for end users.

**Non-Goals:**

- Code signing / notarization in the first cut (document as follow-up if needed).
- Cross-compiling all targets from one runner; use native `ubuntu` / `windows` / `macos` runners.
- Changing the on-disk save format or moving saves into the package directory.
- App store distribution.

## Decisions

### D1 — Native runners per OS, not cross-compile

**Choice:** `ubuntu-latest`, `windows-latest`, and `macos-latest` (Apple Silicon) each build their own artifacts.

**Why:** Tauri + SDL + WebView toolchains are painful to cross-compile; native runners match how developers already build.

**Rejected:** Single Linux job cross-compiling Win/mac; QEMU/cross toolchains for the first release pipeline.

### D2 — Tag trigger: `v*`

**Choice:** Publish Releases on pushes of tags matching `v*` (e.g. `v0.1.0`). Workflow uses `softprops/action-gh-release` (or equivalent) with `GITHUB_TOKEN`.

**Why:** Conventional, explicit, avoids publishing every main commit.

**Rejected:** Release on every push to main; manual `workflow_dispatch`-only (keep `workflow_dispatch` as optional extra for dry runs).

### D3 — Shared packaging contract, thin per-OS wrappers

**Choice:** Extend `scripts/package_release.sh` into a documented packaging contract (stage dir layout + zip/tar name). Add Windows (PowerShell) and macOS helpers or CI-inline steps that produce the same logical layout:

```
gen3recomp-<ver>-<os>-<arch>/
  gen3recomp-player(.bat|.sh)   # entrypoint → launcher with GEN3RECOMP_HOST set
  bin/gen3recomp[.exe]
  bin/gen3recomp-launcher[.exe]
  bin/gba_recompile[.exe]       # when available
  roms/README.txt               # empty placeholder; no dumps
  scripts/                      # cart AOT helpers where applicable
  docs/                         # player guide excerpt + LICENSE pointer
  LICENSE
```

**Why:** Keeps one mental model; CI and local Linux packaging stay aligned.

**Rejected:** Shipping only Tauri’s native installers without a sibling host binary; separate unrelated layouts per OS.

### D4 — `roms/` beside package root; also `build/roms` for developers

**Choice:** Define **install root** as the directory containing the player entrypoint (Release) or the directory containing the host binary’s parent when under `build/` (so `build/roms`). Update launcher `roms_dirs` / BIOS resolution to scan, in order:

1. Install root / package root `roms/` (and `gba_bios.bin` at install root)
2. Host-binary parent `roms/` (covers `bin/../roms` and `build/roms`)
3. CWD `roms/` and repo-root `roms/` (developer convenience)

Add ROM still copies into the preferred writable `roms/` (prefer install/package root when present).

**Why:** Matches the user’s “ROMs inside the build/package folder” request and the existing launcher-ui wording about “beside the executable”.

**Rejected:** Moving user dumps into user-data only; requiring CWD to be the package root forever.

### D5 — Saves stay in user-data; document, don’t relocate

**Choice:** Keep `<user-data>/saves/<sha1>.sav`. Document OS roots:

| OS | User-data default |
|----|-------------------|
| Linux | `~/.local/share/gen3recomp` (`XDG_DATA_HOME` honored) |
| Windows | `%APPDATA%\gen3recomp` |
| macOS | `~/Library/Application Support/gen3recomp` |

**Why:** Survives package upgrades/deletes; already implemented and tested.

**Rejected:** Storing `.sav` next to ROMs in the package (fragile; easy to lose on re-download).

### D6 — macOS artifact arch: arm64 primary on `macos-latest`

**Choice:** Ship `macos-arm64` from GitHub’s Apple Silicon runners. Document that Intel Mac is best-effort / future unless a second macOS job is added.

**Why:** Matches current GitHub hosted runner default; ABI tag already exists (`abi3-macos-arm64`).

**Rejected:** Universal binary in MVP CI (extra complexity).

### D7 — Windows cart Build remains bash-dependent for now

**Choice:** Document that cart AOT Build on Windows still expects Git Bash/WSL for `scripts/build_cart_artifact.sh`. Release still ships the host + launcher so Play works once an artifact exists.

**Why:** Native Windows AOT scripts are out of scope (proposal non-goal).

## Risks / Trade-offs

- [CI flake / missing SDL or WebView deps on runners] → Pin install steps in the workflow; fail the job with clear logs; document runner package lists.
- [Large Tauri/Rust cold builds] → Cache Cargo/npm; accept longer first release jobs.
- [Unsigned Windows/macOS binaries trigger OS warnings] → Document “allow anyway”; signing is a follow-up.
- [Path changes break developers who only used CWD `roms/`] → Keep CWD/repo roots in the scan list (D4).
- [Windows packaging diverges from Linux script] → Enforce a short layout checklist in CI (must contain `bin/` host+launcher and `roms/`).

## Migration Plan

1. Land path-resolution + packaging layout changes; verify local Linux zip still works.
2. Add workflow with `workflow_dispatch` dry-run before enabling tag publish.
3. Cut a `v*` tag to publish the first multi-OS Release.
4. Point README “Download” / player flow at Releases; keep source-build instructions for contributors.

Rollback: delete the workflow and revert packaging/path commits; existing manual Linux script behavior remains the fallback.

## Open Questions

- Whether to also publish a `macos-x64` artifact via a second runner image (defer unless users request it).
- Whether code signing certificates will be available before the first public tag (defer; docs warn about Gatekeeper/SmartScreen).
