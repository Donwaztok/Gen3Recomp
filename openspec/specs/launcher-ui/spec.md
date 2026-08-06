# launcher-ui Specification

## Purpose
Provides a player-facing window so users can select a catalogued ROM and BIOS, complete the one-time cart AOT gate, and start gameplay without using the CLI.

## Requirements

### Requirement: Bare launch opens a floating visual launcher
When the application starts without `--rom`, it MUST open the player launcher UI. For the default player path, that UI MUST be the separate Tauri launcher application (component-based library) in a compact floating window (not maximized by default) with a visual title grid. It MUST NOT present only the debug-text keyboard list as the primary UI, and MUST NOT exit solely with usage text requiring `--rom`.

#### Scenario: Double-click / bare executable
- **WHEN** the user runs the documented player entrypoint with no ROM argument
- **THEN** the Tauri launcher window appears for ROM selection and AOT status with a cover grid for catalogued dumps

#### Scenario: Host CLI still skips UI
- **WHEN** the user runs `gen3recomp --rom <path>`
- **THEN** startup proceeds on the CLI path without requiring the Tauri launcher

### Requirement: Catalogued ROMs from roms folder
The launcher UI MUST list `.gba` files found in a documented `roms/` directory (beside the executable and/or current working directory) that match the MVP catalog by SHA-1. Unknown dumps MUST be shown as rejected or omitted with a clear reason when the user tries to add them.

#### Scenario: Valid Emerald in roms/
- **GIVEN** a catalogued Emerald USA dump in `roms/`
- **WHEN** the launcher UI opens
- **THEN** that dump appears as a selectable title with its catalog display name

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to select or add it
- **THEN** the UI explains the dump is not supported and does not enable Play

### Requirement: Cover grid layout
Catalogued dumps discovered for the launcher MUST be shown in a grid of cover tiles (image + title + AOT readiness cue). The UI MUST NOT require the user to read a monospace text list as the only selection surface.

#### Scenario: Multiple USA titles listed
- **GIVEN** Emerald, Ruby, and Sapphire dumps are available in `roms/`
- **WHEN** the launcher opens
- **THEN** each appears as a distinct grid tile the user can select with the mouse

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

### Requirement: Missing cover does not block play
If a cover image cannot be loaded, the launcher MUST show a placeholder tile and MUST still allow selection, Build, and Play according to BIOS/AOT rules.

#### Scenario: Offline with empty cover cache
- **GIVEN** no cached cover and no network
- **WHEN** the launcher lists a catalogued dump
- **THEN** a placeholder tile is shown and Play remains governed only by BIOS and AOT readiness

### Requirement: Add ROM via file picker
The launcher UI MUST offer an action to add a ROM through a native file picker. Adding MUST copy or reference the file in a way the user understands; the app MUST NOT silently pick the first file in a folder as the active game.

#### Scenario: Explicit choice among multiple dumps
- **GIVEN** more than one catalogued dump is listed
- **WHEN** the user has not selected one
- **THEN** Play remains unavailable until a dump is selected

### Requirement: BIOS status visible
The launcher UI MUST show whether a valid BIOS is available (`--bios`, `./gba_bios.bin`, or an equivalent documented location) and MUST block Play until BIOS requirements for LLE boot are satisfied.

#### Scenario: Missing BIOS
- **GIVEN** no valid BIOS file
- **WHEN** the user selects a catalogued ROM
- **THEN** the UI indicates BIOS is required and Play stays disabled

### Requirement: AOT gate before Play
For the selected dump, Play MUST stay disabled until a valid cart AOT artifact for that SHA-1 is ready (or an equivalent provider-reported static coverage state). The UI MUST explain that a one-time recompile may be required and MUST offer a Build / Recompile action that reports progress or failure.

#### Scenario: First-time dump needs build
- **GIVEN** a selected catalogued ROM with no cart artifact
- **WHEN** the launcher shows status
- **THEN** Play is disabled and a Build action is offered with an explanation that recompilation is required once

#### Scenario: Build succeeds then Play unlocks
- **GIVEN** Build completes successfully for the selected dump
- **WHEN** the artifact is ready
- **THEN** Play becomes enabled without requiring the user to run CMake manually

#### Scenario: Artifact already present
- **GIVEN** a valid cart artifact for the selected SHA-1
- **WHEN** the launcher opens
- **THEN** Play is enabled without forcing a rebuild

### Requirement: Play starts the native session
Choosing Play MUST start the same native boot path as a successful CLI launch for that ROM and BIOS.

#### Scenario: Play Emerald
- **GIVEN** catalogued Emerald, valid BIOS, and ready cart AOT
- **WHEN** the user activates Play
- **THEN** the game session starts through the provider without an external emulator

### Requirement: Existing launcher behaviors retained
BIOS status visibility, AOT gate (Play disabled until ready), `roms/` discovery, Add ROM via file picker, mods access, and no silent first-file autoplay MUST remain true under the new UI.

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to add it
- **THEN** the UI explains the dump is not supported and does not enable Play for it

### Requirement: SDL textured grid is not the default player UI
The custom SDL cover-grid launcher MUST NOT be required as the default bare-launch experience once the Tauri launcher is the documented player entrypoint. Contributors MAY retain a host-side fallback for diagnostics, but player docs MUST point at the Tauri app.

#### Scenario: README points at Tauri launcher
- **WHEN** a newcomer reads the player flow in the top-level README
- **THEN** the documented default is the Tauri launcher, not the SDL textured grid

### Requirement: Product brand reads Gen3Recomp
The Tauri launcher hero brand, HTML document title, and native window title MUST present the product name as **Gen3Recomp** (PascalCase compound). Lowercase `gen3recomp` MUST NOT be the primary visible brand string in those surfaces.

#### Scenario: First viewport brand
- **WHEN** the launcher window opens
- **THEN** the hero brand text reads Gen3Recomp (accent styling on the Recomp segment is allowed)

#### Scenario: OS window title
- **WHEN** the launcher window is shown in the desktop environment
- **THEN** the window title is Gen3Recomp

### Requirement: Brand uses a modern sans display face
The hero brand MUST use a modern sans-serif display typeface appropriate to the dark launcher shell. The previous serif brand face (Fraunces) MUST NOT remain the brand font.

#### Scenario: Brand is not serif Fraunces
- **WHEN** a contributor inspects the launcher stylesheet for the brand class
- **THEN** Fraunces is not the brand `font-family`, and a sans display face is applied instead
