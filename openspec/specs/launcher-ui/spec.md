# launcher-ui Specification

## Purpose
Provides a player-facing window so users can select a catalogued ROM and BIOS, complete the one-time cart AOT gate, and start gameplay without using the CLI.
## Requirements
### Requirement: Launcher UI opens when ROM is omitted
When the application starts without `--rom`, it MUST open the launcher UI and MUST NOT exit solely with usage text requiring `--rom`.

#### Scenario: Double-click / bare executable
- **WHEN** the user runs `gen3recomp` with no arguments
- **THEN** a launcher window appears for ROM selection and AOT status

### Requirement: Catalogued ROMs from roms folder
The launcher UI MUST list `.gba` files found in a documented `roms/` directory (beside the executable and/or current working directory) that match the MVP catalog by SHA-1. Unknown dumps MUST be shown as rejected or omitted with a clear reason when the user tries to add them.

#### Scenario: Valid Emerald in roms/
- **GIVEN** a catalogued Emerald USA dump in `roms/`
- **WHEN** the launcher UI opens
- **THEN** that dump appears as a selectable title with its catalog display name

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to select or add it
- **THEN** the UI explains the dump is not supported and does not enable Play

### Requirement: Add ROM via file picker
The launcher UI MUST offer an action to add a ROM through a native file picker. Adding MUST copy or reference the file in a way the user understands; the app MUST NOT silently pick the first file in a folder as the active game.

#### Scenario: Explicit choice among multiple dumps
- **GIVEN** more than one catalogued dump is listed
- **WHEN** the user has not selected one
- **THEN** Play remains unavailable until a dump is selected

### Requirement: BIOS status visible
The launcher UI MUST show whether a valid BIOS is available (`--bios`, `./gba_bios.bin`, or an equivalent documented location) and MUST block Play until BIOS requirements for LLE boot are satisfied.

#### Scenario: Missing BIOS
- **GIVEN** no valid BIOS file
- **WHEN** the user selects a catalogued ROM
- **THEN** the UI indicates BIOS is required and Play stays disabled

### Requirement: AOT gate before Play
For the selected dump, Play MUST stay disabled until a valid cart AOT artifact for that SHA-1 is ready (or an equivalent provider-reported static coverage state). The UI MUST explain that a one-time recompile may be required and MUST offer a Build / Recompile action that reports progress or failure.

#### Scenario: First-time dump needs build
- **GIVEN** a selected catalogued ROM with no cart artifact
- **WHEN** the launcher shows status
- **THEN** Play is disabled and a Build action is offered with an explanation that recompilation is required once

#### Scenario: Build succeeds then Play unlocks
- **GIVEN** Build completes successfully for the selected dump
- **WHEN** the artifact is ready
- **THEN** Play becomes enabled without requiring the user to run CMake manually

#### Scenario: Artifact already present
- **GIVEN** a valid cart artifact for the selected SHA-1
- **WHEN** the launcher opens
- **THEN** Play is enabled without forcing a rebuild

### Requirement: Play starts the native session
Choosing Play MUST start the same native boot path as a successful CLI launch for that ROM and BIOS.

#### Scenario: Play Emerald
- **GIVEN** catalogued Emerald, valid BIOS, and ready cart AOT
- **WHEN** the user activates Play
- **THEN** the game session starts through the provider without an external emulator
