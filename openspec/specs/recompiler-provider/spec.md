## Purpose

Isolates how native execution is prepared so gba-recomp can be used now and replaced later without rewriting the host.

## Requirements

### Requirement: Provider prepare uses Game Definition data
The recompiler provider MUST accept ROM bytes or path, validated BIOS input, and a Game Definition, then prepare an execution artifact or fail with a clear error.

#### Scenario: Prepare succeeds for a catalogued dump
- **GIVEN** a catalogued ROM, valid BIOS, and matching Game Definition
- **WHEN** the gba-recomp provider prepares execution
- **THEN** it returns a usable session backend or artifact and does not require title-specific logic in runtime

#### Scenario: Prepare failure is visible
- **WHEN** upstream preparation fails
- **THEN** the application reports a human-readable error and does not pretend a session is running

### Requirement: Adapter isolation
Only the gba adapter area MAY depend on gba-recomp. Host modules MUST depend on gen3recomp provider/session contracts.

#### Scenario: Leak check
- **WHEN** `src/app`, `src/runtime`, `src/platform`, `src/video`, `src/audio`, `src/input`, and `src/game` are inspected
- **THEN** they do not include gba-recomp headers

### Requirement: No ROM-derived sources in git
Preparation MUST write any generated or cache artifacts outside the source tree (user data or build/work directories that are gitignored).

#### Scenario: Clean tree after prepare
- **WHEN** provider prepare runs on a developer machine
- **THEN** no ROM-derived generated sources are created as version-controlled files

### Requirement: Compile-time provider wiring
The MVP MUST wire the gba-recomp provider at compile time. The application MUST NOT load arbitrary plugin binaries.

#### Scenario: No plugin scan
- **WHEN** the application starts
- **THEN** it does not discover recompiler providers from external plugin files
