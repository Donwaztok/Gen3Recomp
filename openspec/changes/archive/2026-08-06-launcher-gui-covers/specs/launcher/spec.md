## Purpose

Default bare launch continues to open the launcher rather than requiring `--rom`.

## MODIFIED Requirements

### Requirement: Bare launch opens launcher UI
When `--rom` is omitted, the application MUST open the launcher UI capability (visual floating grid) rather than exiting only with a usage error that `--rom` is required.

#### Scenario: No arguments
- **WHEN** the user runs `gen3recomp` with no arguments
- **THEN** the launcher UI starts successfully

### Requirement: CLI path unchanged when `--rom` is set
When `--rom` is provided, the application MUST keep the existing CLI identification and boot flow without requiring the launcher UI.

#### Scenario: Scripted launch
- **WHEN** the user passes `--rom` and a valid BIOS
- **THEN** startup proceeds on the CLI path without blocking on the launcher window
