## ADDED Requirements

### Requirement: Cover overlays expose Play or Build icons
Each catalogued cover tile MUST show an icon-only primary action overlaid on the cover art (or placeholder): **Play** when that dump’s cart AOT is ready (and existing BIOS/host gates allow Play), or **Build** when cart AOT is missing. The control MUST NOT rely on visible text on the button face; it MUST still expose an accessible name (for example `aria-label`).

#### Scenario: AOT-ready tile shows Play icon
- **GIVEN** a catalogued dump with ready cart AOT, valid BIOS, and host available
- **WHEN** the library grid renders that tile
- **THEN** an icon-only Play control is visible on that cover
- **WHEN** the user activates that Play icon
- **THEN** the native host starts for that dump (same gates as footer Play)

#### Scenario: Missing AOT tile shows Build icon
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the library grid renders that tile
- **THEN** an icon-only Build control is visible on that cover
- **WHEN** the user activates that Build icon
- **THEN** the cart AOT build runs for that dump

#### Scenario: Tile click still selects
- **GIVEN** multiple catalogued dumps in the grid
- **WHEN** the user clicks the cover area outside the action icon
- **THEN** that dump becomes the selected tile without starting Play or Build
