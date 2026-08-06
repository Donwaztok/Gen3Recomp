## ADDED Requirements

### Requirement: Play startup wait is distinct from Build
The launcher MUST present Build as the one-time cart AOT step and MUST NOT imply that every subsequent Play will take several minutes after AOT is ready. When AOT is ready and caches are warm, the UI MUST treat Play as the fast path to the host session (subject to normal process start), not a second long compile gate.

#### Scenario: Ready title offers Play without multi-minute warning
- **GIVEN** a selected catalogued dump with cart AOT ready
- **WHEN** the library footer/status is shown
- **THEN** messaging does not describe Play as another multi-minute recompile
- **THEN** any remaining first-session warm note (empty heal cache) is clearly separate from Build progress text
