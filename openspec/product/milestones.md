# Milestones

Each milestone is one OpenSpec change. Acceptance criteria live in that change’s delta specs (scenarios) and are summarized here.

| ID | Change | Runnable artifact | Exit criterion |
|----|--------|-------------------|----------------|
| M01 | `m01-project-skeleton` | `gen3recomp` builds and prints version/help-equivalent identity | Fresh clone builds with documented CMake commands |
| M02 | `m02-build-and-test-harness` | `gen3recomp_tests` runs under CTest; logging works | Smoke tests pass in CI-local invocation |
| M03 | `m03-launcher-entry` | App accepts `--rom` or opens a file dialog; errors are readable | Missing ROM / bad path fail with non-zero exit |
| M04 | `m04-rom-and-bios-validation` | Known USA R/S/E dumps identify; others are rejected; BIOS validated | Wrong hash never proceeds to runtime |
| M05 | `m05-sdl3-host-window` | A window appears and can be closed | Manual: open + close; automated: platform init/shutdown test |
| M06 | `m06-runtime-session` | Null session runs the host loop until quit | Automated session start/stop test |
| M07 | `m07-recompiler-provider-gba` | gba-recomp adapter prepares a session for a validated dump without leaking upstream types outward | Unit/integration test of provider seam with a fixture or guarded local ROM path |
| M08 | `m08-native-game-boot` | Emerald USA reaches title screen; then Ruby + Sapphire USA | Manual boot checklist + automated “session started with real backend” smoke where feasible |

## Milestone rules

1. Implement only one change at a time.
2. Archive M0N before applying M0N+1, unless a spec correction must land first.
3. If upstream reality contradicts a design, update the change artifacts before coding around them.
4. No milestone may add future.md scope.

## Suggested first implementation slice after architecture sign-off

Start with **M01**, not M07. Resist pulling gba-recomp into the tree until M07.
