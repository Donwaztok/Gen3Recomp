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
The launcher UI MUST list `.gba` files found in a documented `roms/` directory that match the MVP catalog by SHA-1. Discovery MUST include `roms/` beside the player package root and/or beside the host binary (including a `build/roms` layout when the host lives under `build/`), and MAY also scan the current working directory and repo-root `roms/` for developers. Unknown dumps MUST be shown as rejected or omitted with a clear reason when the user tries to add them.

#### Scenario: Valid Emerald in package roms/
- **GIVEN** a catalogued Emerald USA dump in the package’s `roms/` directory beside the player install root
- **WHEN** the launcher UI opens from that package
- **THEN** that dump appears as a selectable title with its catalog display name

#### Scenario: Valid Emerald in build/roms
- **GIVEN** a catalogued Emerald USA dump in `build/roms` next to the built host binary
- **WHEN** the launcher UI opens with that host as the play target
- **THEN** that dump appears as a selectable title

#### Scenario: Unknown dump rejected
- **GIVEN** a `.gba` whose SHA-1 is not in the catalog
- **WHEN** the user tries to select or add it
- **THEN** the UI explains the dump is not supported and does not enable Play

### Requirement: BIOS resolved relative to package root
The launcher MUST look for a valid BIOS at documented locations that include the player package root (and/or the directory containing the host binary), not only the process current working directory, so extracted Release packages work when launched without a special CWD.

#### Scenario: BIOS beside package
- **GIVEN** a valid `gba_bios.bin` at the package root next to the player entrypoint
- **WHEN** the user opens the launcher from that package
- **THEN** BIOS status shows available without requiring the user to `cd` into a specific folder first

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

### Requirement: Play stays available for other ready titles during Build
While a cart AOT Build is running for one catalogued dump, the launcher UI MUST NOT globally disable Play for every title. Play MUST remain available for other dumps that already satisfy BIOS, host, and AOT readiness. The dump currently building MUST keep Play disabled until its artifact is ready (existing AOT gate).

#### Scenario: Play another title while Build runs
- **GIVEN** title A has no cart artifact and title B is AOT-ready with valid BIOS and host
- **WHEN** the user starts Build for title A
- **THEN** Play for title B remains enabled
- **THEN** activating Play for title B starts that session while Build for A continues

#### Scenario: Building title stays ungated until ready
- **GIVEN** Build is in progress for title A
- **WHEN** the user views title A’s cover actions
- **THEN** Play for title A remains disabled until Build succeeds and AOT readiness is reflected

### Requirement: Cover shows Build progress for the building title
While Build runs, the cover tile of the dump being built MUST show a progress indicator (bar and short phase or status text) that updates before the Build command finishes. The indicator MUST apply whether Build was started from that cover or from the footer. Other covers MUST NOT show that dump’s in-progress bar as if they were building.

#### Scenario: Progress appears on the building cover
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the user starts Build for that dump
- **THEN** that dump’s cover shows a progress bar (or equivalent) while Build runs
- **THEN** the phase or status text on that cover updates at least once before completion

#### Scenario: Other covers are not marked as building
- **GIVEN** Build is in progress for title A
- **WHEN** the user views title B’s cover
- **THEN** title B does not display title A’s Build progress bar

#### Scenario: Build completion clears cover progress
- **GIVEN** Build finishes successfully for a dump
- **WHEN** the artifact becomes ready
- **THEN** the in-progress progress indicator is no longer shown on that cover
- **THEN** Play becomes available for that dump per existing AOT and BIOS gates

### Requirement: At most one Build at a time in the UI
The launcher UI MUST prevent starting a second cart AOT Build while one is already running. The user MUST receive a clear indication that a Build is already in progress (for example disabled Build actions elsewhere and/or status text). Concurrent multi-Build is out of scope.

#### Scenario: Second Build blocked while first runs
- **GIVEN** Build is already running for title A
- **WHEN** the user tries to start Build for title B
- **THEN** a second Build does not start
- **THEN** the UI indicates a Build is already in progress

### Requirement: Play startup wait is distinct from Build
The launcher MUST present Build as the one-time cart AOT step and MUST NOT imply that every subsequent Play will take several minutes after AOT is ready. When AOT is ready and caches are warm, the UI MUST treat Play as the fast path to the host session (subject to normal process start), not a second long compile gate.

#### Scenario: Ready title offers Play without multi-minute warning
- **GIVEN** a selected catalogued dump with cart AOT ready
- **WHEN** the library footer/status is shown
- **THEN** messaging does not describe Play as another multi-minute recompile
- **THEN** any remaining first-session warm note (empty heal cache) is clearly separate from Build progress text

### Requirement: Build shows a live progress bar
While a cart AOT Build is running, the launcher UI MUST display a progress bar and a short phase or status label that updates before the build command finishes. The indicator MUST appear for Build started from either the cover action or the footer. When compile unit counts are known, the bar SHOULD be determinate (0–100% or n/N); when only a phase is known, an indeterminate or busy bar with phase text is acceptable.

#### Scenario: Build in progress updates the bar
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the user starts Build
- **THEN** a progress bar is visible while the build runs
- **THEN** the status/phase text changes at least once before completion (for example generating → compiling → linking)

#### Scenario: Build completion clears the in-progress bar
- **GIVEN** a Build that finishes successfully
- **WHEN** the artifact becomes ready
- **THEN** the in-progress progress bar is no longer shown as an active build
- **THEN** the UI reflects AOT ready (Play available per existing gates)

### Requirement: Cover overlays expose Play or Build icons
Each catalogued cover tile MUST show an icon-only primary action overlaid on the cover art (or placeholder): **Play** when that dump’s cart AOT is ready (and existing BIOS/host gates allow Play), or **Build** when cart AOT is missing. The control MUST NOT rely on visible text on the button face; it MUST still expose an accessible name (for example `aria-label`).

#### Scenario: AOT-ready tile shows Play icon
- **GIVEN** a catalogued dump with ready cart AOT, valid BIOS, and host available
- **WHEN** the library grid renders that tile
- **THEN** an icon-only Play control is visible on that cover
- **WHEN** the user activates that Play icon
- **THEN** the native host starts for that dump (same gates as footer Play)

#### Scenario: Missing AOT tile shows Build icon
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the library grid renders that tile
- **THEN** an icon-only Build control is visible on that cover
- **WHEN** the user activates that Build icon
- **THEN** the cart AOT build runs for that dump

#### Scenario: Tile click still selects
- **GIVEN** multiple catalogued dumps in the grid
- **WHEN** the user clicks the cover area outside the action icon
- **THEN** that dump becomes the selected tile without starting Play or Build
