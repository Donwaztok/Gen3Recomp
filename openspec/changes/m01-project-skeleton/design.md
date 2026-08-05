## Context

See proposal.md. Architecture module map is in `openspec/product/architecture.md`. There is no existing CMake tree.

## Goals / Non-Goals

**Goals:**
- Smallest C++20 CMake project that matches the agreed directory names.
- One product target.
- Placeholder module directories so later milestones do not rename the tree.
- Basic repo hygiene: gitignore build dirs, ROM/BIOS patterns, generated output.

**Non-Goals:**
- SDL3, fmt/spdlog, Catch2, gba-recomp.
- Real CLI parsing beyond “run and print identity”.
- Installing libraries or exporting CMake packages.
- CI matrix for every OS.

## Decisions

- **Root CMake + add_subdirectory per module** even if some modules only contain a stub translation unit. Alternative: one `src/` glob. Rejected because it hides module boundaries.
- **Stub `main.cpp` calls into `app`**. Alternative: all code in `main.cpp`. Rejected because App is the orchestration module from M03 onward.
- **`--version` is a trivial argv check** in M01. Full CLI parsing waits for M03.
- **Version is a compile definition or tiny generated header**, kept trivial (`0.1.0-dev` is fine).
- **License file (D11):** `LICENSE` is PolyForm Noncommercial 1.0.0, same text family as gba-recomp, with gen3recomp copyright. README already states this. A third-party attribution file can wait until M07 pins upstream.

## Risks / Trade-offs

- [Empty stub modules feel like ceremony] → Keep stubs to one tiny `.cpp`/CMakeLists each, or a single `src/app` + `src/core` if other folders would be empty files with no symbols. Prefer creating a directory only when it has a compilation unit. Document the full map in architecture; M01 may create `core` + `app` only.
- [Overbuilding CMake functions] → No custom framework. Plain `add_library` + `target_link_libraries`.

## Migration Plan

Not applicable. First code milestone.

## Open Questions

- Exact CMake minimum version (3.20 vs 3.24) can be chosen at apply time based on SDL3 later needs; record it in README.
