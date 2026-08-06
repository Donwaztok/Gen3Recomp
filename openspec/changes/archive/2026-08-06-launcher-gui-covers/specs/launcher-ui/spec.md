## Purpose

Player-facing floating launcher with a mouse-driven cover grid, AOT gate controls, and cross-platform host presentation.

## MODIFIED Requirements

### Requirement: Bare launch opens a floating visual launcher
When the application starts without `--rom`, it MUST open a compact floating launcher window (not maximized by default) with a visual title grid. It MUST NOT present only the debug-text keyboard list as the primary UI.

#### Scenario: Double-click / bare executable
- **WHEN** the user runs `gen3recomp` with no arguments
- **THEN** a floating launcher window appears with a cover grid for catalogued dumps

### Requirement: Mouse-driven selection and actions
The launcher UI MUST support mouse click to select a title and to activate primary actions (Play when enabled, Build when offered, Add ROM, Mods). Keyboard shortcuts MAY remain as secondary input.

#### Scenario: Click to select and Play
- **GIVEN** a catalogued dump with ready cart AOT and valid BIOS
- **WHEN** the user clicks the title’s cover and then Play
- **THEN** the native session starts for that dump

#### Scenario: Click Build
- **GIVEN** a selected catalogued dump without a ready cart artifact
- **WHEN** the user clicks Build
- **THEN** the AOT build pipeline runs with visible progress or error text in the UI

### Requirement: Cover grid layout
Catalogued dumps discovered for the launcher MUST be shown in a grid of cover tiles (image + title + AOT readiness cue). The UI MUST NOT require the user to read a monospace text list as the only selection surface.

#### Scenario: Multiple USA titles listed
- **GIVEN** Emerald, Ruby, and Sapphire dumps are available in `roms/`
- **WHEN** the launcher opens
- **THEN** each appears as a distinct grid tile the user can select with the mouse

### Requirement: Missing cover does not block play
If a cover image cannot be loaded, the launcher MUST show a placeholder tile and MUST still allow selection, Build, and Play according to BIOS/AOT rules.

#### Scenario: Offline with empty cover cache
- **GIVEN** no cached cover and no network
- **WHEN** the launcher lists a catalogued dump
- **THEN** a placeholder tile is shown and Play remains governed only by BIOS and AOT readiness

### Requirement: Existing launcher behaviors retained
BIOS status visibility, AOT gate (Play disabled until ready), `roms/` discovery, Add ROM via file picker, mods access, and no silent first-file autoplay MUST remain true under the new UI.

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to add it
- **THEN** the UI explains the dump is not supported and does not enable Play for it
