# native-boot Specification

## Purpose
Defines MVP success: user-supplied USA dumps of Ruby, Sapphire, and Emerald boot natively through the BIOS intro to the title screen.
## Requirements
### Requirement: Emerald USA reaches the title screen
With a catalogued Emerald USA ROM and valid BIOS, the application MUST execute native/recompiled/interpreted game code through the BIOS intro and reach the title screen.

#### Scenario: Emerald boot
- **GIVEN** valid Emerald USA ROM and BIOS
- **WHEN** the user launches gen3recomp
- **THEN** the BIOS intro plays and the Emerald title screen becomes visible without using an external emulator

### Requirement: Ruby and Sapphire USA reach the title screen
The same host path MUST boot catalogued Ruby USA and Sapphire USA dumps to their title screens.

#### Scenario: Ruby boot
- **GIVEN** valid Ruby USA ROM and BIOS
- **WHEN** the user launches gen3recomp
- **THEN** the title screen becomes visible

#### Scenario: Sapphire boot
- **GIVEN** valid Sapphire USA ROM and BIOS
- **WHEN** the user launches gen3recomp
- **THEN** the title screen becomes visible

### Requirement: One host path for all MVP titles
Boot MUST use the same application, runtime, and provider seam. Title differences MUST come from Game Definitions and adapter data, not duplicated executables.

#### Scenario: Same binary
- **WHEN** a user boots Ruby then Emerald
- **THEN** both runs use the `gen3recomp` product executable

