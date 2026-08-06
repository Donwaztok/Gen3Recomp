## ADDED Requirements

### Requirement: Play stays available for other ready titles during Build
While a cart AOT Build is running for one catalogued dump, the launcher UI MUST NOT globally disable Play for every title. Play MUST remain available for other dumps that already satisfy BIOS, host, and AOT readiness. The dump currently building MUST keep Play disabled until its artifact is ready (existing AOT gate).

#### Scenario: Play another title while Build runs
- **GIVEN** title A has no cart artifact and title B is AOT-ready with valid BIOS and host
- **WHEN** the user starts Build for title A
- **THEN** Play for title B remains enabled
- **THEN** activating Play for title B starts that session while Build for A continues

#### Scenario: Building title stays ungated until ready
- **GIVEN** Build is in progress for title A
- **WHEN** the user views title A’s cover actions
- **THEN** Play for title A remains disabled until Build succeeds and AOT readiness is reflected

### Requirement: Cover shows Build progress for the building title
While Build runs, the cover tile of the dump being built MUST show a progress indicator (bar and short phase or status text) that updates before the Build command finishes. The indicator MUST apply whether Build was started from that cover or from the footer. Other covers MUST NOT show that dump’s in-progress bar as if they were building.

#### Scenario: Progress appears on the building cover
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the user starts Build for that dump
- **THEN** that dump’s cover shows a progress bar (or equivalent) while Build runs
- **THEN** the phase or status text on that cover updates at least once before completion

#### Scenario: Other covers are not marked as building
- **GIVEN** Build is in progress for title A
- **WHEN** the user views title B’s cover
- **THEN** title B does not display title A’s Build progress bar

#### Scenario: Build completion clears cover progress
- **GIVEN** Build finishes successfully for a dump
- **WHEN** the artifact becomes ready
- **THEN** the in-progress progress indicator is no longer shown on that cover
- **THEN** Play becomes available for that dump per existing AOT and BIOS gates

### Requirement: At most one Build at a time in the UI
The launcher UI MUST prevent starting a second cart AOT Build while one is already running. The user MUST receive a clear indication that a Build is already in progress (for example disabled Build actions elsewhere and/or status text). Concurrent multi-Build is out of scope.

#### Scenario: Second Build blocked while first runs
- **GIVEN** Build is already running for title A
- **WHEN** the user tries to start Build for title B
- **THEN** a second Build does not start
- **THEN** the UI indicates a Build is already in progress
