## MODIFIED Requirements

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
