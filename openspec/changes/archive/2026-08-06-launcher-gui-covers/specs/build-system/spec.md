## Purpose

Ensures the host executable and launcher can be built on the supported desktop platforms.

## MODIFIED Requirements

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
