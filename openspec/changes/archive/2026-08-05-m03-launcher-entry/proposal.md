## Why

Users need a way to start the app and point it at a ROM without a game runtime existing yet. A thin launcher makes later validation and boot steps observable and testable.

## What Changes

- Add CLI parsing for `--rom`, `--bios`, `--help`, and existing `--version`.
- If `--rom` is omitted, open one native file dialog; if the user cancels, exit non-zero without crashing.
- Print human-readable errors for missing files.
- Establish stable exit codes for success, usage error, and input error.
- Do not validate SHA-1 catalogs yet (M04). Do not open a game window yet (M05).

## Capabilities

### New Capabilities
- `launcher`: Obtains ROM/BIOS paths, reports errors, and exits with documented status codes.

### Modified Capabilities
- (none)

## Impact

- Replaces M01’s temporary no-argument “just print something” UX.
- Introduces `app` orchestration as the process owner.
- May start using SDL3 only for a file dialog; if that pulls SDL3 early, keep usage limited to dialog + init/quit. Opening a game window remains M05. If SDL3 dialog is too heavy, `--rom` becomes mandatory in M03 and the dialog moves to M05. Prefer dialog-in-M03 only if cheap.
