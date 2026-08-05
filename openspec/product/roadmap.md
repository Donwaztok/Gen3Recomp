# Roadmap

Incremental delivery. Each row is a vertical slice that produces a runnable, testable artifact.

```
M01 skeleton → M02 tests/logging → M03 launcher → M04 identify ROM
        → M05 SDL3 window → M06 runtime loop → M07 gba-recomp provider → M08 native boot
```

This order **intentionally differs** from the first sketch (recompiler before window). See [decisions.md](decisions.md) D2.

## Now — Architecture

- OpenSpec product docs and milestone changes
- Agreed module map, dependency rules, and legal constraints
- No implementation yet

## Next — MVP (native boot)

| Milestone | Outcome |
|-----------|---------|
| [M01](../changes/m01-project-skeleton/) | Repo layout + CMake hello `gen3recomp` |
| [M02](../changes/m02-build-and-test-harness/) | Catch2 + spdlog/fmt + passing smoke tests |
| [M03](../changes/m03-launcher-entry/) | CLI / file pick, clear errors, exit codes |
| [M04](../changes/m04-rom-and-bios-validation/) | SHA-1 validation, USA R/S/E identity, BIOS check |
| [M05](../changes/m05-sdl3-host-window/) | SDL3 window opens and closes cleanly |
| [M06](../changes/m06-runtime-session/) | Host loop with a null session backend |
| [M07](../changes/m07-recompiler-provider-gba/) | Isolated gba-recomp provider prepares a session artifact |
| [M08](../changes/m08-native-game-boot/) | At least one title reaches the title screen; then all three USA titles |

## Later — Gen3 completeness (post-MVP)

- FireRed / LeafGreen catalog entries and validation
- EU / JP revisions as additional catalog rows
- Cartridge save polish and host-clock RTC options
- Packaging for Linux / Windows / macOS

## Future — explicitly parked

Tracked only in [future.md](future.md):

- Multiplayer
- Vulkan
- Mods
- Save states
- Plugin system
- Editor
- Networking

## Sync policy

- Pin gba-recomp to an exact revision.
- Review upstream on a scheduled cadence, not continuously.
- Absorb upstream changes only inside `src/recomp/gba/` and `third_party/`.
