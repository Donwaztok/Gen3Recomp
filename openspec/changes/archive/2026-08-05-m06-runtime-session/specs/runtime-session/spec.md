## Purpose

Owns the running lifetime of a game session on the host without embedding title-specific logic or a specific recompiler vendor.

## ADDED Requirements

### Requirement: Session start tick and stop
The runtime MUST provide a session that can be started, ticked until quit, and stopped, releasing host resources.

#### Scenario: Null session runs
- **GIVEN** validated ROM identity and BIOS
- **WHEN** a session starts with the null backend
- **THEN** the host loop runs until quit and stops cleanly

### Requirement: Backend is replaceable
The runtime MUST talk to a session backend contract, not to gba-recomp types. A null backend MUST be sufficient for this milestone.

#### Scenario: Runtime compiles without upstream types
- **WHEN** runtime sources are inspected
- **THEN** they do not include gba-recomp headers

### Requirement: No title-specific runtime behavior
The runtime MUST NOT branch on Ruby, Sapphire, or Emerald. It MAY receive a Game Definition as opaque data to pass to a backend.

#### Scenario: Same loop for every definition
- **WHEN** sessions start for different catalogued titles
- **THEN** runtime control flow differs only by data passed into the backend, not by title switches in runtime code

### Requirement: Quit request ends the session
When the platform reports a quit request, the session MUST stop.

#### Scenario: Window close ends session
- **WHEN** the user closes the window during a null session
- **THEN** the session stops and the process can exit successfully
