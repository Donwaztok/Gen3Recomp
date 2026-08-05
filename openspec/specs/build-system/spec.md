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

### Requirement: Documented build instructions
Project documentation MUST include the minimum commands to configure and build on Linux.

#### Scenario: README build section
- **WHEN** a newcomer opens the top-level README
- **THEN** they can follow documented CMake commands to build `gen3recomp` without reading internal design docs
