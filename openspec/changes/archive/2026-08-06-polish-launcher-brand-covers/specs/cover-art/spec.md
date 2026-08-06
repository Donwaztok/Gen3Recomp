## ADDED Requirements

### Requirement: Per-title cover tiles show front-box art
For each MVP catalog title shown in the Tauri cover grid, the cover tile MUST present a front-box-like crop (or a curated single-title front cover). When a dual Ruby/Sapphire box sheet is the image source, Ruby and Sapphire MUST each crop to their own front panel so the spine or GBA cartridge edge is not the dominant face of the tile. Emerald MUST continue to show a clean front cover.

#### Scenario: Ruby tile is not mid-spine
- **GIVEN** a catalogued Ruby USA dump with a successfully loaded cover image
- **WHEN** the launcher grid renders the Ruby tile
- **THEN** the visible cover area is dominated by Ruby front-box art, not the dual-box spine or Sapphire panel

#### Scenario: Sapphire tile is not mid-spine
- **GIVEN** a catalogued Sapphire USA dump with a successfully loaded cover image
- **WHEN** the launcher grid renders the Sapphire tile
- **THEN** the visible cover area is dominated by Sapphire front-box art, not the dual-box spine or Ruby panel

#### Scenario: Emerald remains a clean front cover
- **GIVEN** a catalogued Emerald USA dump with a successfully loaded cover image
- **WHEN** the launcher grid renders the Emerald tile
- **THEN** the tile shows Emerald front-box art without an obvious mid-box crop artifact
