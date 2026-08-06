## Purpose

Defines how a user starts gen3recomp and supplies ROM and BIOS paths before identification or gameplay exists.
## Requirements
### Requirement: ROM path via CLI
The application MUST accept a ROM filesystem path through a `--rom` option.

#### Scenario: Valid path is accepted
- **GIVEN** a filesystem path that points to an existing file
- **WHEN** the user runs `gen3recomp --rom <path>`
- **THEN** the launcher accepts the path and continues startup

#### Scenario: Missing file is rejected
- **WHEN** the user runs `gen3recomp --rom` with a path that does not exist
- **THEN** the application prints a human-readable error and exits with a non-zero input-error status

### Requirement: Optional BIOS path via CLI
The application MUST accept an optional `--bios` path. If provided and the file does not exist, startup MUST fail. If omitted, startup MAY continue until BIOS validation exists.

#### Scenario: Missing BIOS file is rejected
- **WHEN** the user passes `--bios` with a path that does not exist
- **THEN** the application prints a human-readable error and exits non-zero

### Requirement: Help text
`gen3recomp --help` MUST print usage that mentions `--rom`, `--bios`, and `--version`, then exit successfully. Usage MAY mention optional `--prepare` only as a secondary/diagnostic option and MUST NOT present it as the way to obtain full-cart native speed.

#### Scenario: Help invocation
- **WHEN** the user runs `gen3recomp --help`
- **THEN** usage text is printed and the process exits with status 0
- **THEN** `--rom` and `--bios` are documented

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

### Requirement: Documented exit codes
The launcher MUST use distinct non-zero exit codes for usage errors versus input errors, documented in `--help` or README.

#### Scenario: Unknown flag
- **WHEN** the user passes an unknown option
- **THEN** the application exits with the usage-error status and prints help or an error mentioning valid options

### Requirement: Default BIOS path beside working directory
When `--bios` is omitted, the launcher MUST look for `gba_bios.bin` in the current working directory before failing BIOS validation. It MUST NOT invent or download a BIOS.

#### Scenario: Local gba_bios.bin is used
- **GIVEN** `./gba_bios.bin` exists and is a valid catalogued BIOS
- **WHEN** the user runs `gen3recomp --rom <path>` without `--bios`
- **THEN** startup uses that BIOS file

### Requirement: No silent first-file ROM autoplay
The launcher MUST NOT start a session by silently picking the first `.gba` file in a directory. If multiple catalogued dumps are presented in one selection flow, the user MUST choose explicitly.

#### Scenario: Ambiguous multi-dump choice
- **GIVEN** more than one catalogued MVP dump is available to a selection UI or helper
- **WHEN** the user has not chosen among them
- **THEN** the application does not start a game session until one dump is selected or `--rom` is supplied

### Requirement: Prepare is not the primary warm path
`--prepare` (if retained) MUST be documented as optional/diagnostic. Help and README MUST prioritize local cart AOT for full-speed play.

#### Scenario: Help mentions AOT over prepare
- **WHEN** the user runs `gen3recomp --help`
- **THEN** usage still documents `--rom`, `--bios`, and `--version`
- **THEN** if `--prepare` appears, it is not presented as the required path to full-speed gameplay

