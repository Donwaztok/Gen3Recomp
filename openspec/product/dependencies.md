# Dependencies

## First-party runtime dependencies (MVP)

| Dependency | Role | Notes |
|------------|------|-------|
| C++20 compiler | Language | clang or gcc; MSVC only if it stays simple |
| CMake 3.20+ (floor to be confirmed in M01) | Build | One project, two targets |
| SDL3 | Platform window, events, audio device, dialogs | Only backend |
| fmt | Formatting | May be used via spdlog |
| spdlog | Logging | Wrap init in `core` |
| Catch2 v3 | Tests | `gen3recomp_tests` only |

## Recompiler backend

| Dependency | Role | Constraint |
|------------|------|------------|
| [gba-recomp](https://github.com/mstan/gbarecomp) | Static recomp + GBA hardware runtime + interpreter/JIT | PolyForm Noncommercial 1.0.0. Pin exact git revision. Touch only from `src/recomp/gba/`. gen3recomp uses the same license (D11). |

Related upstream game repos are **references**, not dependencies:

- [EmeraldRecomp](https://github.com/mstan/EmeraldRecomp)
- [RubySapphireRecomp](https://github.com/mstan/RubySapphireRecomp)
- [FireRedLeafGreenRecomp](https://github.com/mstan/FireRedLeafGreenRecomp)

Use them to learn catalog hashes, symbol metadata needs, and boot expectations. Do not vendor their game-specific host code wholesale.

## User-supplied inputs (not distributed)

| Input | Required | Validation |
|-------|----------|------------|
| Game ROM (`.gba`) | Yes | SHA-1 against catalog |
| GBA BIOS | Yes | SHA-1 against known BIOS catalog |
| In-game save files | Created at runtime | Stored in user data dir |
| Recomp heal/cache files | Created at runtime | Stored in user data dir, keyed by ROM identity |

## External knowledge sources (non-code)

Allowed as data/metadata inputs when legally clean:

- public SHA-1 lists for known good dumps (verify before entering catalog)
- symbol maps / function bounds from pret decomp projects (names + addresses only)
- GBATEK / emulator hardware docs via upstream, not reimplemented here

Not allowed:

- pret C source as an execution oracle
- Nintendo assets, logos, music, graphics
- redistributing BIOS or ROMs

## Sync strategy

1. Pin gba-recomp.
2. Record the pin in CMake and in a short `third_party/README.md` (created in M07).
3. Upstream upgrades are a dedicated OpenSpec change, not drive-by edits.
4. If upstream host glue collides with our Platform module, prefer calling into upstream hardware/runtime and keeping *our* SDL loop — or wrapping theirs behind the session backend — whichever is smaller. Decide in M07 design if still ambiguous after reading the pinned revision.

## Versioning posture

- gen3recomp starts at 0.1.x during bring-up.
- Breaking provider seam changes are allowed before 1.0 because there is one adapter and no plugin ABI.
