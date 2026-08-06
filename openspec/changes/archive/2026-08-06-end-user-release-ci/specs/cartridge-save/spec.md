## ADDED Requirements

### Requirement: Documented cartridge save locations
End-user documentation MUST describe that MVP persistence is in-game cartridge saves only (no save states), that save files are keyed by ROM SHA-1, and the default save directory on Linux, Windows, and macOS.

#### Scenario: Reader finds save paths
- **WHEN** a player opens the README or player guide
- **THEN** they can locate `<user-data>/saves/<sha1>.sav` and the OS-specific user-data roots for Linux, Windows, and macOS

#### Scenario: Docs exclude save states
- **WHEN** a player reads the documented save section
- **THEN** save states are not presented as an available MVP feature
