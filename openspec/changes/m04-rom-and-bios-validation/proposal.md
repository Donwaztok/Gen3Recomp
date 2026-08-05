## Why

Native boot is unsafe and pointless if any `.gba` can enter the pipeline. Exact dump identity is the product’s trust boundary and the only way to select the correct Game Definition.

## What Changes

- Add a Game Definition catalog for Ruby USA, Sapphire USA, and Emerald USA.
- Compute SHA-1 of the ROM and identify a definition or reject.
- Require and validate a GBA BIOS against a known BIOS hash catalog.
- Stop startup with clear errors on mismatch.
- Do not start gba-recomp or a window.

## Capabilities

### New Capabilities
- `game-identification`: Catalog + SHA-1 identity for supported titles.
- `bios-validation`: Known GBA BIOS is required and verified.

### Modified Capabilities
- (none)

## Impact

- Introduces the `game` module for real.
- Uses `core` SHA-1 and file reads.
- Changes successful launcher flow from “print paths and exit” to “identify or die, then print identity and exit” until later milestones continue.
