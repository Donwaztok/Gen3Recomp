# EPICs

EPICs group milestones. They are planning buckets, not CMake targets.

## EPIC-0 — Specification baseline

**Intent:** Agree on architecture, legal stance, and incremental plan before code.

**Includes:** this `openspec/` tree.

**Done when:** Maintainers accept vision, architecture, decisions, and M01–M08 changes.

## EPIC-1 — Bring-up shell

**Intent:** A boring, solid C++ project that builds, logs, and tests.

**Milestones:** M01, M02

**Features:** F-BUILD, F-TEST, F-LOG

## EPIC-2 — Trust the cartridge

**Intent:** Users can point the app at files and get a definitive identity or a definitive rejection.

**Milestones:** M03, M04

**Features:** F-LAUNCH, F-IDENTIFY, F-BIOS

## EPIC-3 — Host surface

**Intent:** A game-agnostic windowed host that can run a session loop.

**Milestones:** M05, M06

**Features:** F-WINDOW, F-SESSION

## EPIC-4 — Native Gen3 execution

**Intent:** Isolated gba-recomp integration and actual boot of the three MVP titles.

**Milestones:** M07, M08

**Features:** F-PROVIDER, F-BOOT, F-AUDIO, F-INPUT, F-SAVE-CART

## EPIC-5 — Post-MVP Gen3 expansion (not started)

**Intent:** FireRed / LeafGreen and additional regions as catalog growth, not new hosts.

**Depends on:** EPIC-4 done and stable.

**Features (future):** F-FRLG, F-REGIONS

## EPIC-F — Parked vision

See [future.md](future.md). These EPICs must not spawn MVP tasks.
