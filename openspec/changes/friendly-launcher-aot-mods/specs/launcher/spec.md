## ADDED Requirements

### Requirement: Bare launch opens launcher UI
When `--rom` is omitted, the application MUST open the launcher UI capability rather than exiting only with a usage error that `--rom` is required.

#### Scenario: No arguments
- **WHEN** the user runs `gen3recomp` with no options
- **THEN** the launcher UI starts successfully

### Requirement: CLI path remains available
When `--rom` is provided, the application MUST keep the existing CLI identification and boot flow without requiring the launcher UI.

#### Scenario: Explicit ROM flag
- **WHEN** the user runs `gen3recomp --rom <catalogued-path> --bios <path>`
- **THEN** startup proceeds on the CLI path without blocking on the launcher window

## MODIFIED Requirements

### Requirement: ROM selection when omitted
When `--rom` is omitted, the application MUST open the launcher UI so the user can select or add a catalogued dump. It MUST NOT hang with no output, and MUST NOT silently autoplay the first `.gba` file found.

#### Scenario: Cancelled picker or refused omission
- **WHEN** the user starts the app without `--rom` and cancels a picker or closes the launcher without playing
- **THEN** the process exits non-zero or cleanly without starting a game session

#### Scenario: Cancelled launcher
- **WHEN** the user starts the app without `--rom` and closes the launcher without playing
- **THEN** the process exits without starting a game session

#### Scenario: No silent first-file
- **GIVEN** multiple `.gba` files under `roms/`
- **WHEN** the launcher lists them
- **THEN** none is started until the user explicitly selects one and activates Play
