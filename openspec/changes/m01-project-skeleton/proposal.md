## Why

The repository has architecture docs but no buildable software. Contributors need a tiny, conventional C++/CMake skeleton so every later milestone lands in a known layout instead of inventing structure ad hoc.

## What Changes

- Add the agreed source layout (`src/core`, `src/app`, …) as empty-or-minimal placeholders.
- Add root CMake that builds one product target: `gen3recomp`.
- Make the application print a version/identity line and exit 0.
- Add legal/doc stubs required to start a public noncommercial project (PolyForm NC LICENSE, CONTRIBUTING pointer, gitignore for build/ROM/BIOS).
- Do not add SDL3, gba-recomp, Catch2, or launcher behavior yet.

## Capabilities

### New Capabilities
- `build-system`: CMake project produces the single product executable and documents how to build it.

### Modified Capabilities
- (none)

## Impact

- Creates the first compilable tree.
- Establishes directory names used by all later changes.
- Touches root docs/license/gitignore only as needed for a clone-and-build workflow.
