## Why

The MVP is only real when the games boot natively. This milestone connects audio and input, runs the BIOS path, and accepts title-screen boot for Emerald first, then Ruby and Sapphire.

## What Changes

- Complete host audio + input adapters.
- Run the real backend through BIOS intro to the title screen.
- Enable cartridge save persistence in user data via provider save hardware.
- Verify Ruby USA, Sapphire USA, and Emerald USA.
- Keep future features (save states, mods, Vulkan, FRLG) untouched.

## Capabilities

### New Capabilities
- `native-boot`: Supported titles reach the title screen natively.
- `host-audio`: PCM from the backend reaches the host audio device.
- `host-input`: Host controls map to GBA buttons.
- `cartridge-save`: In-game saves persist through the provider save model.

### Modified Capabilities
- (none)

## Impact

- First playable slice.
- Manual acceptance becomes primary; automate only what is stable.
