## Context

M01–M02 give a binary, logs, and tests. No catalog yet.

## Goals / Non-Goals

**Goals:**
- Tiny CLI.
- Deterministic exit codes.
- Tests for missing file / unknown flag / help / version.
- Clear orchestration function in `app`: parse args → resolve paths → return a launch request object.

**Non-Goals:**
- SHA-1 identification.
- GUI launcher shell.
- Config file profiles.
- Remembering last ROM path (nice later, not now).

## Decisions

- **Flag style:** GNU long options only (`--rom`, `--bios`, `--help`, `--version`). Short flags optional if they stay trivial.
- **File dialog:** Deferred to M05. M03 requires `--rom` on the CLI so SDL3 is not pulled in early.
- **Exit codes:** `0` ok, `2` usage, `1` input/runtime error is enough. Do not invent a large errno table.
- **After successful path resolution in M03:** print the resolved ROM path (and BIOS if any) and exit 0. That keeps the milestone runnable without lying about booting a game. M04+ continue past this point.

## Risks / Trade-offs

- [SDL3 only for a dialog feels early] → Defer dialog.
- [Printing path and exiting will change in M04] → Expected; launcher still owns parse/report.

## Migration Plan

Update README usage. Existing `--version` remains.

## Open Questions

- Resolved: M03 requires `--rom`; M05 may add a picker.
