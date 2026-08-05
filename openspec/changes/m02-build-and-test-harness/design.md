## Context

M01 produced a product binary. This change adds verification and diagnostics only.

## Goals / Non-Goals

**Goals:**
- Catch2 v3 via CMake (FetchContent or similar).
- CTest integration.
- spdlog + fmt initialized in `core`.
- One smoke test that will not be deleted later (e.g. version string non-empty, or SHA-1 of a known fixture string).

**Non-Goals:**
- Coverage tooling, sanitizer matrices, GUI test harnesses.
- Full ROM validation (M04).
- Logging to rotating files unless it is literally fewer lines than stderr logging.

## Decisions

- **stderr logging by default.** Files can wait.
- **Do not introduce a Util module** for logging wrappers. `core` owns logger init.
- **Prefer Catch2's CMake package** over vendoring a giant tree by hand.
- **SHA-1:** if adding it now keeps M04 smaller, put a pure function in `core` and test it with the empty-string or `"abc"` official vectors. If that expands M02 too much, wait for M04.

## Risks / Trade-offs

- [FetchContent without pin] → Pin Catch2/spdlog/fmt tags.
- [spdlog pulling fmt conflicts] → Use spdlog’s bundled or external fmt consistently.

## Migration Plan

Update README build/test section. No user data migration.

## Open Questions

None that block the task list.
