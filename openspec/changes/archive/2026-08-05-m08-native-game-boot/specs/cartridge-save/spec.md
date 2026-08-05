## Purpose

Allows in-game saving through the cartridge save hardware modeled by the provider, without implementing host save states.

## ADDED Requirements

### Requirement: In-game saves persist
When a supported title writes cartridge save data, the application MUST persist that data in the user data directory and reload it on a later launch of the same dump identity.

#### Scenario: Save and relaunch
- **GIVEN** a running supported title
- **WHEN** the user performs an in-game save and later launches the same ROM identity again
- **THEN** the provider-backed save data is available to the game

### Requirement: Saves are per dump identity
Save files MUST be keyed by ROM identity (SHA-1 or stable Game Definition id) so titles do not overwrite each other.

#### Scenario: Ruby does not consume Emerald save
- **WHEN** the user has saved in Emerald and then launches Ruby
- **THEN** Ruby does not load the Emerald save file

### Requirement: No save-state feature in MVP
The application MUST NOT expose save-state slots as an MVP feature even if upstream supports them.

#### Scenario: MVP surface
- **WHEN** a user inspects MVP documentation and controls
- **THEN** only in-game cartridge saves are described
