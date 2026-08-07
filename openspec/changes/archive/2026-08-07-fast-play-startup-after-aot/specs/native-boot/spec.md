## ADDED Requirements

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
