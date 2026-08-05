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
`gen3recomp --help` MUST print usage that mentions `--rom`, `--bios`, and `--version`, then exit successfully.

#### Scenario: Help invocation
- **WHEN** the user runs `gen3recomp --help`
- **THEN** usage text is printed and the process exits with status 0

### Requirement: ROM selection when omitted
When `--rom` is omitted, the application MUST either open a single native file picker or print usage that states `--rom` is required. It MUST NOT hang with no output.

#### Scenario: Cancelled picker or refused omission
- **WHEN** the user starts the app without `--rom` and cancels a picker or the app requires CLI input
- **THEN** the process exits non-zero and does not start a game session

### Requirement: Documented exit codes
The launcher MUST use distinct non-zero exit codes for usage errors versus input errors, documented in `--help` or README.

#### Scenario: Unknown flag
- **WHEN** the user passes an unknown option
- **THEN** the application exits with the usage-error status and prints help or an error mentioning valid options
