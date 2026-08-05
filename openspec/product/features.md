# Features

Feature IDs are stable references for EPICs, milestones, and tests.

## MVP features

| ID | Feature | Epic | Change | Summary acceptance |
|----|---------|------|--------|--------------------|
| F-BUILD | Product build | EPIC-1 | M01 | `cmake --build` produces `gen3recomp` |
| F-TEST | Automated tests | EPIC-1 | M02 | `ctest` runs Catch2 smoke tests |
| F-LOG | Structured logging | EPIC-1 | M02 | App can log info/error without crashing |
| F-LAUNCH | Launcher entry | EPIC-2 | M03 | CLI ROM path or single file dialog; clear errors |
| F-IDENTIFY | ROM identity | EPIC-2 | M04 | SHA-1 maps to Ruby/Sapphire/Emerald USA or reject |
| F-BIOS | BIOS validation | EPIC-2 | M04 | Known BIOS accepted; invalid BIOS blocks boot |
| F-WINDOW | Host window | EPIC-3 | M05 | SDL3 window opens; close ends the process cleanly |
| F-SESSION | Runtime session | EPIC-3 | M06 | Session starts, ticks, stops; null backend allowed |
| F-PROVIDER | Recompiler provider seam | EPIC-4 | M07 | Host talks only to gen3recomp contracts; gba adapter is isolated |
| F-BOOT | Native boot | EPIC-4 | M08 | BIOS intro through title screen for MVP titles |
| F-AUDIO | Host audio | EPIC-4 | M08 | Backend samples reach the host device (or an explicit silent-but-wired path with a loud log if device init fails) |
| F-INPUT | Host input | EPIC-4 | M08 | Keyboard (and gamepad if trivial) maps to GBA buttons |
| F-SAVE-CART | Cartridge save | EPIC-4 | M08 | In-game save uses provider save chip model + user-data file; no save states |

## Explicit non-features (MVP)

| ID | Item | Notes |
|----|------|-------|
| NF-FRLG | FireRed / LeafGreen | Catalog later |
| NF-NET | Multiplayer / networking | Future |
| NF-VK | Vulkan renderer | Future |
| NF-MOD | Mods / `.gbamod` | Future; upstream has this, we do not expose it |
| NF-SS | Save states | Future |
| NF-PLUG | Plugin ABI | Never for MVP; maybe never |
| NF-EDIT | Editors / debugging studio | Future |
| NF-WIDE | Adaptive widescreen | Future; upstream has this |

Detailed Given/When/Then acceptance criteria are the scenarios inside each change’s `specs/**/spec.md`.
