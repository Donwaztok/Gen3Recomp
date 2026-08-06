## ADDED Requirements

### Requirement: Documented Tauri launcher build
Project documentation MUST include the minimum commands to install tooling and build the Tauri + React launcher on Linux, and MUST note Windows and macOS requirements (Node, Rust, platform WebView).

#### Scenario: README launcher build section
- **WHEN** a newcomer opens the top-level README
- **THEN** they can follow documented commands to build the launcher alongside the CMake host

### Requirement: Player packaging excludes illegal payloads
Any documented player package that includes the Tauri launcher and/or `gen3recomp` MUST NOT include ROM dumps, BIOS images, cover image blobs, or prebuilt cart AOT objects.

#### Scenario: Release checklist
- **WHEN** a maintainer prepares a Release zip per project docs
- **THEN** the package contains host/launcher/tools only — no dumps, BIOS, covers, or cart artifacts

## MODIFIED Requirements

### Requirement: Documented build instructions
Project documentation MUST include the minimum commands to configure and build the CMake host on Linux, and MUST document how the Tauri launcher relates to that host for the player path.

#### Scenario: README build section
- **WHEN** a newcomer opens the top-level README
- **THEN** they can follow documented CMake commands to build `gen3recomp` and documented commands to build the Tauri launcher without reading internal design docs
