## Purpose

Lets players discover and enable user-supplied mod packages from a local mods directory without distributing Nintendo ROMs, BIOS images, or ROM-derived game binaries.

## ADDED Requirements

### Requirement: Discover mods from a user mods directory
The application MUST scan a documented `mods/` directory (user data and/or beside the executable) for mod packages that declare a manifest. Packages without a valid manifest MUST be ignored or listed as invalid with a reason.

#### Scenario: Valid package appears in the list
- **GIVEN** a mod package with a valid manifest under `mods/`
- **WHEN** the mod manager refreshes
- **THEN** the mod appears with its display name and enable state

### Requirement: Enable and disable mods
The user MUST be able to enable or disable each discovered mod from the launcher UI. Enablement MUST persist across launches.

#### Scenario: Disable persists
- **GIVEN** a mod was disabled in the launcher
- **WHEN** the application restarts
- **THEN** that mod remains disabled until the user enables it again

### Requirement: Mods must not embed ROM or BIOS bytes
The mod manager MUST reject packages that include `.gba` dumps, BIOS images, or other Nintendo ROM-derived binary payloads as package content.

#### Scenario: Package with embedded ROM rejected
- **GIVEN** a mod package that contains a `.gba` file as distributed content
- **WHEN** the manager validates the package
- **THEN** the mod is not enableable and the UI states the reason

### Requirement: Enabled mods apply at prepare time
When starting a session, prepare MUST receive the set of enabled mods so the gba adapter can apply supported hooks/patches. Unsupported mod features MUST fail closed with a readable error rather than silently ignoring enablement.

#### Scenario: Enabled mod reaches prepare
- **GIVEN** at least one enabled valid mod
- **WHEN** the user starts Play
- **THEN** prepare is invoked with that mod set visible to the provider seam

### Requirement: In-game enablement when available
If the session can expose a pause or overlay control without a third-party plugin ABI, the application MUST offer an in-game control that toggles mod enablement consistently with launcher state. If that control cannot be implemented in this change, the launcher MUST remain the supported control surface and documentation MUST state that in-game control is not yet available.

#### Scenario: Launcher remains authoritative if in-game unavailable
- **WHEN** in-game mod controls are not implemented
- **THEN** enablement is still fully controllable from the launcher UI
- **THEN** documentation states that in-game control is not yet available
