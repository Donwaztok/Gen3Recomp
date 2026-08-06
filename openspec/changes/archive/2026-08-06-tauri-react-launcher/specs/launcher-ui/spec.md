## MODIFIED Requirements

### Requirement: Launcher UI opens when ROM is omitted
When the application starts without `--rom`, it MUST open the player launcher UI. For the default player path, that UI MUST be the separate Tauri launcher application (component-based library). The process MUST NOT exit solely with usage text requiring `--rom`.

#### Scenario: Double-click / bare executable
- **WHEN** the user runs the documented player entrypoint with no ROM argument
- **THEN** the Tauri launcher window appears for ROM selection and AOT status

#### Scenario: Host CLI still skips UI
- **WHEN** the user runs `gen3recomp --rom <path>`
- **THEN** startup proceeds on the CLI path without requiring the Tauri launcher

## ADDED Requirements

### Requirement: SDL textured grid is not the default player UI
The custom SDL cover-grid launcher MUST NOT be required as the default bare-launch experience once the Tauri launcher is the documented player entrypoint. Contributors MAY retain a host-side fallback for diagnostics, but player docs MUST point at the Tauri app.

#### Scenario: README points at Tauri launcher
- **WHEN** a newcomer reads the player flow in the top-level README
- **THEN** the documented default is the Tauri launcher, not the SDL textured grid
