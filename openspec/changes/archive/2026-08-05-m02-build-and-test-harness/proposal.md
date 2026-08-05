## Why

A skeleton without tests and logging will rot. Before launcher logic exists, the project needs Catch2, CTest, fmt, spdlog, and a smoke test that proves the harness works.

## What Changes

- Add Catch2 test target `gen3recomp_tests`.
- Register tests with CTest.
- Add fmt and spdlog; initialize logging from `core`.
- Document test commands in README.
- Optionally add a tiny tested `core` helper if it stays small.

## Capabilities

### New Capabilities
- `test-harness`: Catch2 + CTest smoke tests are part of the build.
- `diagnostics`: The application can emit structured log output through a single logging setup.

### Modified Capabilities
- (none)

## Impact

- Adds test and logging dependencies.
- Extends CMake.
- No user-facing launcher behavior yet.
