## 1. Dependencies

- [ ] 1.1 Add pinned fmt and spdlog to CMake
- [ ] 1.2 Add pinned Catch2 v3 to CMake
- [ ] 1.3 Initialize logging in `core` and use it from `app` startup

## 2. Tests

- [ ] 2.1 Create `tests/` with a Catch2 main or Catch2-provided main
- [ ] 2.2 Add `gen3recomp_tests` target and `enable_testing()` / `catch_discover_tests` or equivalent
- [ ] 2.3 Add a smoke test that passes on a clean build

## 3. Verification

- [ ] 3.1 Update README with `ctest` instructions
- [ ] 3.2 Run configure, build, and `ctest` locally until green
