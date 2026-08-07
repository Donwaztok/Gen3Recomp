# native-boot Specification

## Purpose
Defines MVP success: user-supplied USA dumps of Ruby, Sapphire, and Emerald boot natively through the BIOS intro to the title screen.
## Requirements
### Requirement: Emerald USA reaches the title screen
With a catalogued Emerald USA ROM, valid BIOS, and local static cart AOT present (developer corpus or user-data cart artifact), the application MUST execute native game code through the BIOS intro to the title screen at playable speed. Without static cart AOT, boot MAY still reach the title screen via self-heal but MUST NOT be documented as the full-speed product path.

#### Scenario: Emerald boot with static AOT
- **GIVEN** valid Emerald USA ROM, valid BIOS, and static cart AOT for that dump
- **WHEN** the user launches gen3recomp
- **THEN** the BIOS intro plays and the Emerald title screen becomes visible without using an external emulator
- **THEN** title and early post-title ROM code do not require per-PC cart compilation on first visit

#### Scenario: Emerald boot without static AOT
- **GIVEN** valid Emerald USA ROM and BIOS but empty cart dispatch
- **WHEN** the user launches gen3recomp
- **THEN** the session may still progress via self-heal
- **THEN** the product docs do not claim this path is full-speed for the whole game

### Requirement: Ruby and Sapphire USA reach the title screen
The same host path MUST boot catalogued Ruby USA and Sapphire USA dumps to their title screens. When static cart AOT exists for those dumps, boot MUST follow the same full-speed expectations as Emerald.

#### Scenario: Ruby boot
- **GIVEN** valid Ruby USA ROM and BIOS
- **WHEN** the user launches gen3recomp
- **THEN** the title screen becomes visible

#### Scenario: Sapphire boot
- **GIVEN** valid Sapphire USA ROM and BIOS
- **WHEN** the user launches gen3recomp
- **THEN** the title screen becomes visible

#### Scenario: Ruby or Sapphire with static AOT
- **GIVEN** static cart AOT for a catalogued Ruby or Sapphire USA dump
- **WHEN** the user launches that dump
- **THEN** title-path ROM code runs with static coverage active for cart PCs

### Requirement: One host path for all MVP titles
Boot MUST use the same application, runtime, and provider seam. Title differences MUST come from Game Definitions and adapter data, not duplicated executables.

#### Scenario: Same binary
- **WHEN** a user boots Ruby then Emerald
- **THEN** both runs use the `gen3recomp` product executable

### Requirement: Launcher Play reaches title screen for catalogued dumps
After a successful AOT gate in the launcher UI, Play MUST boot catalogued USA Ruby, Sapphire, or Emerald dumps through the same host path to the title screen at playable speed when static cart coverage is active.

#### Scenario: Emerald from launcher
- **GIVEN** Emerald USA selected, BIOS valid, cart artifact ready
- **WHEN** the user activates Play
- **THEN** the BIOS intro and title screen become reachable without an external emulator

### Requirement: Warm repeated Play reaches early boot quickly
When a catalogued dump already has a valid cart AOT artifact and a populated self-heal overlay cache under user data, starting Play (or an equivalent host launch) MUST reach first guest frames / BIOS intro without a minute-scale host-startup stall on a typical warm local SSD. Cold empty heal cache MAY still pay first-time overlay compile cost; that MUST NOT be required on every subsequent Play once the cache is warm.

#### Scenario: Second Play after warm cache
- **GIVEN** Emerald USA with ready cart AOT and an already-populated heal cache from a prior successful session
- **WHEN** the user activates Play again in a new host process
- **THEN** early boot becomes visible in seconds rather than on the order of one to two minutes of pre-guest stall

#### Scenario: Empty heal cache is a distinct case
- **GIVEN** ready cart AOT but an empty or missing heal cache
- **WHEN** the user activates Play for the first time on that machine
- **THEN** a longer first-session overlay cost MAY occur
- **THEN** a later Play with the same warm cache MUST follow the fast repeated-Play expectation above

