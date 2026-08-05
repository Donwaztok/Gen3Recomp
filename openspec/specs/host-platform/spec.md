## Purpose

Provides a desktop window and event loop through SDL3 so later runtime sessions have a host surface that does not know about Pokémon or recompilers.

## Requirements

### Requirement: Window opens after successful validation
After a ROM is identified and the BIOS is validated, the application MUST open a desktop window using the SDL3 platform backend.

#### Scenario: Valid inputs open a window
- **GIVEN** a catalogued ROM and valid BIOS
- **WHEN** startup proceeds past validation
- **THEN** a window appears with a stable title that does not require Nintendo assets

### Requirement: Quit closes cleanly
The application MUST leave the main loop and shut down the platform backend when the user closes the window or issues a quit event.

#### Scenario: User closes the window
- **WHEN** the user closes the host window
- **THEN** SDL/platform resources are released and the process exits successfully

### Requirement: Platform stays game-agnostic
The platform module MUST NOT depend on Game Definitions, Pokémon titles, or recompiler providers.

#### Scenario: Platform boundary
- **WHEN** platform code is inspected or tested
- **THEN** it only deals with windowing, events, time, and similar host services
