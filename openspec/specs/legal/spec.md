## Purpose

Defines the legal boundary of gen3recomp: the project is a host for user-supplied dumps and must never become a Nintendo data distribution channel.

## Requirements

### Requirement: No Nintendo data in the project
The project MUST NOT contain, generate into git, or distribute Game Boy Advance ROMs, Nintendo GBA BIOS images, extracted game assets, or ROM-derived generated source files.

#### Scenario: Clean repository tree
- **WHEN** a contributor inspects the version-controlled tree
- **THEN** no `.gba` ROM, BIOS binary, Nintendo asset pack, or checked-in recompiled ROM C++ is present

#### Scenario: Runtime inputs stay local
- **WHEN** a user supplies a ROM or BIOS to run a game
- **THEN** those files remain user-local inputs and are not copied into the project repository by default tooling

### Requirement: Unsupported dumps are refused
The system MUST refuse to run a ROM that is not in the supported identity catalog.

#### Scenario: Unknown SHA-1
- **WHEN** the user provides a ROM whose SHA-1 is not catalogued
- **THEN** the application exits with a failure and does not start gameplay

### Requirement: Honest attribution and licensing
Project documentation MUST state that the project is licensed under PolyForm Noncommercial 1.0.0, that Pokémon and related marks belong to their owners, and that users must supply legally obtained dumps.

#### Scenario: README states the boundary
- **WHEN** a newcomer reads the top-level project documentation
- **THEN** they are told ROMs/BIOS are not included and that the project is noncommercial PolyForm NC
