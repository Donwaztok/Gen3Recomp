# tauri-launcher Specification

## Purpose

Provides the player-facing desktop shell as a separate Tauri application with a React component UI that orchestrates ROM selection, covers, AOT build, mods, and launching the native host.

## Requirements

### Requirement: Separate launcher application
The project MUST provide a separate launcher application (distinct from the gameplay host process) that presents the player library UI. Closing the launcher without Play MUST NOT start a game session.

#### Scenario: Open launcher without playing
- **GIVEN** the player starts the documented launcher entrypoint
- **WHEN** they close the launcher window without activating Play
- **THEN** no `gen3recomp` game session is started

### Requirement: Component-based floating library UI
The launcher UI MUST use a component library (not a custom SDL debug or hand-drawn rect grid as the primary chrome). The window MUST be a compact floating desktop window (not a maximized kiosk). Catalogued titles MUST appear in a visual cover grid with mouse selection.

#### Scenario: Cover grid with mouse select
- **GIVEN** at least one catalogued dump is available
- **WHEN** the launcher opens
- **THEN** titles are shown in a cover grid and a click selects a title

### Requirement: Surface BIOS, AOT, and Play gates
For the selected dump, the launcher MUST show BIOS readiness and cart AOT readiness, MUST offer Build when AOT is missing, and MUST enable Play only when BIOS and AOT requirements are satisfied (same gates as the host player path).

#### Scenario: Play disabled without AOT
- **GIVEN** a selected catalogued dump with valid BIOS but no cart artifact
- **WHEN** the library view renders
- **THEN** Play is disabled and Build is offered

#### Scenario: Play spawns host
- **GIVEN** selected dump, valid BIOS, and ready cart AOT
- **WHEN** the user activates Play
- **THEN** the launcher starts the native host for that ROM and BIOS without requiring the user to type CLI flags

### Requirement: Covers from cache without shipping art
The launcher MUST display per-title covers from user-data cache and/or documented local overrides, with placeholders on miss/failure. It MUST NOT require cover image blobs in the git tree or Release zip.

#### Scenario: Offline placeholder
- **GIVEN** no cached cover and no network
- **WHEN** a title tile renders
- **THEN** a placeholder is shown and the title remains selectable

### Requirement: Mods management in launcher
The launcher MUST allow viewing discovered mods and enabling/disabling valid packages without embedding ROM or BIOS payloads in mod packages (same policy as the host mod manager).

#### Scenario: Toggle mod
- **GIVEN** a valid mod package under the documented mods path
- **WHEN** the user enables it in the launcher
- **THEN** enablement persists for subsequent launches

### Requirement: Add ROM via picker
The launcher MUST offer Add ROM via a native file picker and MUST reject unknown SHA-1 dumps with a clear message without enabling Play for them.

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` not in the MVP catalog
- **WHEN** the user adds it through the picker
- **THEN** the UI explains it is unsupported and Play stays disabled for that file
