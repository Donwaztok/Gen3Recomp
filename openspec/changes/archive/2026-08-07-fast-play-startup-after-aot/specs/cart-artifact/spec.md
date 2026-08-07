## ADDED Requirements

### Requirement: Repeated artifact activation stays cheap
Loading an already-built cart artifact for a catalogued SHA-1 MUST NOT impose a minute-scale cost on every Play. The activation path MUST prefer a strategy suitable for repeated sessions (for example deferred symbol binding or an equivalent measured fast path) while still providing correct static cart coverage for the session.

#### Scenario: Play with existing libcart
- **GIVEN** a valid user-data cart artifact for the dump’s SHA-1
- **WHEN** prepare activates that artifact at session start
- **THEN** activation completes without a minute-scale stall attributable to cart loading alone
- **THEN** diagnostics identify cart-activation time separately from overlay warm-load and guest boot
