## ADDED Requirements

### Requirement: Launcher Play reaches title screen for catalogued dumps
After a successful AOT gate in the launcher UI, Play MUST boot catalogued USA Ruby, Sapphire, or Emerald dumps through the same host path to the title screen at playable speed when static cart coverage is active.

#### Scenario: Emerald from launcher
- **GIVEN** Emerald USA selected, BIOS valid, cart artifact ready
- **WHEN** the user activates Play
- **THEN** the BIOS intro and title screen become reachable without an external emulator
