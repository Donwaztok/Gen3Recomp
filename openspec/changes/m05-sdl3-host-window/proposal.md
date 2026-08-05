## Why

Native boot needs a host window. Creating that window before gba-recomp keeps platform bring-up debuggable and prevents the backend integration from owning SDL.

## What Changes

- Add SDL3 as the first `platform` backend.
- Open a window after successful identification + BIOS validation.
- Present a clear-color or test pattern framebuffer through `video`.
- Close on window close / quit event and exit 0.
- Do not run game code yet.

## Capabilities

### New Capabilities
- `host-platform`: SDL3 window, event pump, and shutdown.
- `host-video`: Game-agnostic presentation of a framebuffer or clear color.

### Modified Capabilities
- (none)

## Impact

- First visible GUI.
- Introduces `platform` and `video` modules.
- Successful M04 path no longer exits immediately after printing identity.
