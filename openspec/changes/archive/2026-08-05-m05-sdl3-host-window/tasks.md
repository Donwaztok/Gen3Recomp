## 1. SDL3 platform

- [x] 1.1 Add SDL3 to CMake and document the dependency
- [x] 1.2 Implement platform init, window create, event poll, and shutdown
- [x] 1.3 Keep platform free of `game` and `recomp` includes

## 2. Video placeholder

- [x] 2.1 Present a clear color or test framebuffer at 240×160 logical size
- [x] 2.2 Integer-scale to the window

## 3. App wiring and verification

- [x] 3.1 After successful M04 validation, open the window loop instead of exiting
- [x] 3.2 Quit cleanly on window close
- [x] 3.3 Add the best automated test that does not flake in headless environments
- [x] 3.4 Manually confirm window open/close
