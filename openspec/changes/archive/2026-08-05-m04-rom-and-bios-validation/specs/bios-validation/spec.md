## Purpose

Requires a user-supplied Game Boy Advance BIOS and verifies it against known good hashes before any boot path that depends on BIOS code.

## ADDED Requirements

### Requirement: BIOS is required to continue past identification
After a ROM is identified, the application MUST require a BIOS path and MUST NOT continue toward runtime without a validated BIOS.

#### Scenario: Missing BIOS after M04
- **GIVEN** a catalogued ROM
- **WHEN** the user does not supply a BIOS file
- **THEN** the application explains that a GBA BIOS is required and exits non-zero

### Requirement: BIOS SHA-1 must match the known catalog
The application MUST compute the SHA-1 of the BIOS file and accept it only if it matches a known official GBA BIOS hash recorded in project data.

#### Scenario: Valid BIOS
- **GIVEN** a BIOS file with a catalogued SHA-1
- **WHEN** validation runs
- **THEN** startup may continue

#### Scenario: Invalid BIOS
- **GIVEN** a BIOS file with a non-catalogued SHA-1
- **WHEN** validation runs
- **THEN** the application prints the computed SHA-1, rejects the file, and does not start a session

### Requirement: BIOS is never distributed
Project docs and tooling MUST continue to treat BIOS files as user-supplied inputs.

#### Scenario: Docs mention user-supplied BIOS
- **WHEN** a user reads startup documentation
- **THEN** they are told to provide their own legally obtained GBA BIOS
