## Context

Launcher can resolve file paths. This milestone adds trust and identity. Hashes must be verified against known-good sources at implementation time — do not invent hashes in docs.

## Goals / Non-Goals

**Goals:**
- One catalog data file (JSON/TOML/YAML — pick one boring format).
- Pure identification function in `game` + SHA-1 in `core`.
- Catch2 tests with tiny fixtures (not real ROMs): feed known byte buffers / hashed temp files.
- BIOS catalog separate from game catalog.

**Non-Goals:**
- Header-based heuristics (“this looks like Emerald”).
- Multiple regions.
- Automatic BIOS search across the whole disk (a small documented default path is allowed if it stays simple, e.g. `./gba_bios.bin` or user-data path).

## Decisions

- **Exact SHA-1 only.** No “close enough”, no CRC-only checks.
- **Canonical USA hashes** recorded with citations in a comment or NOTICE in the catalog file (No-Intro / upstream README, verified locally).
- **Default BIOS lookup (optional):** if `--bios` is omitted, try one or two obvious local paths before failing. Do not scan `$HOME`.
- **After success in M04:** print display name + region + SHA-1 and exit 0 until M05 continues. Same honest slice pattern as M03.

## Risks / Trade-offs

- [Wrong hash in catalog] → Verify dumps yourself before committing hashes; tests use synthetic hashes for logic, not copyrighted blobs.
- [Users with EU dumps feel broken] → Error message should say USA MVP only.

## Migration Plan

README: list supported titles as USA-only and require BIOS.

## Open Questions

None. Hash values themselves are implementation data, not spec text.
