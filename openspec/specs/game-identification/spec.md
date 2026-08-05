## Purpose

Identifies supported Pokémon Generation III dumps from exact SHA-1 catalog data and rejects everything else before any runtime starts.

## Requirements

### Requirement: Catalog contains MVP USA titles
The application MUST include Game Definitions for Pokémon Ruby USA, Pokémon Sapphire USA, and Pokémon Emerald USA, each with a display name, region, stable id, and canonical SHA-1.

#### Scenario: Three identities exist
- **WHEN** the catalog is loaded
- **THEN** it contains exactly the MVP titles above as supported game identities (additional titles MUST NOT be required)

### Requirement: ROM SHA-1 selects a Game Definition
The application MUST compute the SHA-1 of the supplied ROM file and select the matching Game Definition when the hash is catalogued.

#### Scenario: Known Emerald dump
- **GIVEN** a ROM file whose SHA-1 matches the Emerald USA catalog entry
- **WHEN** identification runs
- **THEN** the selected definition is Emerald USA and startup may continue

#### Scenario: Known Ruby dump
- **GIVEN** a ROM file whose SHA-1 matches the Ruby USA catalog entry
- **WHEN** identification runs
- **THEN** the selected definition is Ruby USA

#### Scenario: Known Sapphire dump
- **GIVEN** a ROM file whose SHA-1 matches the Sapphire USA catalog entry
- **WHEN** identification runs
- **THEN** the selected definition is Sapphire USA

### Requirement: Unknown ROM is rejected
If the ROM SHA-1 is not in the catalog, the application MUST print the computed SHA-1, state that the dump is unsupported, and exit non-zero without starting a session.

#### Scenario: Random or modified dump
- **GIVEN** a file that is not a catalogued dump
- **WHEN** the user passes it as `--rom`
- **THEN** identification fails, the SHA-1 appears in the error output, and no game session starts

### Requirement: Game Definition is data only
Identification MUST expose title differences as data fields and MUST NOT require runtime modules to encode title-specific rules.

#### Scenario: Definition fields
- **WHEN** a definition is selected
- **THEN** it includes at least id, display name, region, SHA-1, generation family, and provider hints needed later for save/RTC family
