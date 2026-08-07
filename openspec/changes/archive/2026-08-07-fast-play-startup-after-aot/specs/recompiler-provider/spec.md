## ADDED Requirements

### Requirement: Heal cache warm policy does not block every Play
When a populated overlay heal cache exists, session prepare MUST NOT require eagerly loading every cached overlay shard before guest execution if an on-demand (or otherwise cheaper) load path can supply healed functions when first needed. A diagnostic eager warm-load mode MAY remain available behind an explicit flag or build option.

#### Scenario: On-demand load after prior session
- **GIVEN** thousands of overlay shards already present in the dump’s heal cache
- **WHEN** a new Play session starts with the default player path
- **THEN** prepare does not spend a minute-scale interval dlopening every shard up front
- **THEN** healed functions still become available when first executed (from cache, without recompiling when the shard is valid)

#### Scenario: Diagnostic eager warm remains optional
- **GIVEN** a maintainer enables the documented eager warm-load diagnostic mode
- **WHEN** prepare runs
- **THEN** the runtime MAY still warm-load the full cache for measurement or debugging
