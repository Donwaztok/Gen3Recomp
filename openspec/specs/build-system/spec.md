## Purpose

Establishes that gen3recomp is a single CMake C++20 application that a contributor can configure, build, and run without game backends.

## Requirements

### Requirement: CMake produces gen3recomp
The repository MUST provide a CMake project that builds one product executable named `gen3recomp` using C++20.

#### Scenario: Fresh configure and build
- **GIVEN** a clean checkout and a supported C++20 toolchain
- **WHEN** a contributor configures and builds the project with the documented commands
- **THEN** the build succeeds and produces the `gen3recomp` executable

### Requirement: Version flag identity
`gen3recomp --version` MUST print a version or project identity line to standard output and exit successfully.

#### Scenario: Version invocation
- **WHEN** a contributor runs `gen3recomp --version`
- **THEN** the process prints a recognizable gen3recomp identity/version line and exits with status 0

### Requirement: Host builds on Windows, Linux, and macOS
The CMake build for `gen3recomp` (launcher + CLI shell) MUST succeed on Windows, Linux, and macOS with documented dependencies (SDL3 and platform toolchain). Platform-specific cart AOT scripts MAY differ by OS, but the launcher UI MUST link and run on each.

#### Scenario: Linux developer build
- **WHEN** a developer configures and builds on Linux per README
- **THEN** `gen3recomp` is produced and bare launch opens the launcher UI

#### Scenario: Windows and macOS host build
- **WHEN** a developer follows the documented Windows or macOS build steps
- **THEN** `gen3recomp` is produced without requiring Linux-only headers for the launcher UI path

### Requirement: Documented multi-OS recipes
README or build docs MUST describe how to build on Windows, Linux, and macOS, including SDL3 install notes and any ABI tag differences for cart artifacts.

#### Scenario: Reader finds OS matrix
- **WHEN** a user opens the project build documentation
- **THEN** Windows, Linux, and macOS are each listed with a workable build path

### Requirement: Documented build instructions
Project documentation MUST include the minimum commands to configure and build the CMake host on Linux, and MUST document how the Tauri launcher relates to that host for the player path. Docs MUST also point readers to the multi-OS recipes for Windows and macOS.

#### Scenario: README build section
- **WHEN** a newcomer opens the top-level README
- **THEN** they can follow documented CMake commands to build `gen3recomp` and documented commands to build the Tauri launcher without reading internal design docs

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
