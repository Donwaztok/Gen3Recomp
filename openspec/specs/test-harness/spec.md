## Purpose

Gives contributors a Catch2 and CTest harness so every later milestone can add automated checks next to the product binary.

## Requirements

### Requirement: Catch2 test executable
The build MUST produce a test executable named `gen3recomp_tests` that uses Catch2.

#### Scenario: Test target builds
- **GIVEN** a configured project
- **WHEN** the test target is built
- **THEN** `gen3recomp_tests` is produced without building a second product game binary

### Requirement: CTest runs smoke tests
`ctest` MUST run at least one passing smoke test from the Catch2 suite.

#### Scenario: Default ctest invocation
- **GIVEN** a completed test build
- **WHEN** a contributor runs `ctest` in the build directory
- **THEN** the smoke test runs and passes
