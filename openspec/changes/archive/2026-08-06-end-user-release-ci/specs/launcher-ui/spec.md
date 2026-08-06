## MODIFIED Requirements

### Requirement: Catalogued ROMs from roms folder
The launcher UI MUST list `.gba` files found in a documented `roms/` directory that match the MVP catalog by SHA-1. Discovery MUST include `roms/` beside the player package root and/or beside the host binary (including a `build/roms` layout when the host lives under `build/`), and MAY also scan the current working directory and repo-root `roms/` for developers. Unknown dumps MUST be shown as rejected or omitted with a clear reason when the user tries to add them.

#### Scenario: Valid Emerald in package roms/
- **GIVEN** a catalogued Emerald USA dump in the package’s `roms/` directory beside the player install root
- **WHEN** the launcher UI opens from that package
- **THEN** that dump appears as a selectable title with its catalog display name

#### Scenario: Valid Emerald in build/roms
- **GIVEN** a catalogued Emerald USA dump in `build/roms` next to the built host binary
- **WHEN** the launcher UI opens with that host as the play target
- **THEN** that dump appears as a selectable title

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to select or add it
- **THEN** the UI explains the dump is not supported and does not enable Play

## ADDED Requirements

### Requirement: BIOS resolved relative to package root
The launcher MUST look for a valid BIOS at documented locations that include the player package root (and/or the directory containing the host binary), not only the process current working directory, so extracted Release packages work when launched without a special CWD.

#### Scenario: BIOS beside package
- **GIVEN** a valid `gba_bios.bin` at the package root next to the player entrypoint
- **WHEN** the user opens the launcher from that package
- **THEN** BIOS status shows available without requiring the user to `cd` into a specific folder first
