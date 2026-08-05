## 1. SDL3 platform

- [ ] 1.1 Add SDL3 to CMake and document the dependency
- [ ] 1.2 Implement platform init, window create, event poll, and shutdown
- [ ] 1.3 Keep platform free of `game` and `recomp` includes

## 2. Video placeholder

- [ ] 2.1 Present a clear color or test framebuffer at 240×160 logical size
- [ ] 2.2 Integer-scale to the window

## 3. App wiring and verification

- [ ] 3.1 After successful M04 validation, open the window loop instead of exiting
- [ ] 3.2 Quit cleanly on window close
- [ ] 3.3 Add the best automated test that does not flake in headless environments
- [ ] 3.4 Manually confirm window open/close
