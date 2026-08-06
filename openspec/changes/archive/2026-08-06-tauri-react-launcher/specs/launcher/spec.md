## MODIFIED Requirements

### Requirement: ROM selection when omitted
When `--rom` is omitted, the player path MUST open the Tauri launcher so the user can select or add a catalogued dump. It MUST NOT hang with no output, and MUST NOT silently autoplay the first `.gba` file found.

#### Scenario: Cancelled picker or refused omission
- **WHEN** the user starts the app without `--rom` and cancels a picker or closes the launcher without playing
- **THEN** the process exits non-zero or cleanly without starting a game session

#### Scenario: Cancelled launcher
- **WHEN** the user starts without `--rom` and closes the Tauri launcher without playing
- **THEN** the process exits without starting a game session

#### Scenario: No silent first-file
- **GIVEN** multiple `.gba` files under `roms/`
- **WHEN** the launcher lists them
- **THEN** none is started until the user explicitly selects one and activates Play

### Requirement: Bare launch opens launcher UI
When `--rom` is omitted, the application MUST open the Tauri launcher capability rather than exiting only with a usage error that `--rom` is required.

#### Scenario: No arguments
- **WHEN** the user runs the documented bare player entrypoint with no options
- **THEN** the Tauri launcher starts successfully

### Requirement: CLI path remains available
When `--rom` is provided to `gen3recomp`, the application MUST keep the existing CLI identification and boot flow without requiring the Tauri launcher.

#### Scenario: Explicit ROM flag
- **WHEN** the user runs `gen3recomp --rom <catalogued-path> --bios <path>`
- **THEN** startup proceeds on the CLI path without blocking on the Tauri launcher window
