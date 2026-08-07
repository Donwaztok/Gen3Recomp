## ADDED Requirements

### Requirement: Build shows a live progress bar
While a cart AOT Build is running, the launcher UI MUST display a progress bar and a short phase or status label that updates before the build command finishes. The indicator MUST appear for Build started from either the cover action or the footer. When compile unit counts are known, the bar SHOULD be determinate (0–100% or n/N); when only a phase is known, an indeterminate or busy bar with phase text is acceptable.

#### Scenario: Build in progress updates the bar
- **GIVEN** a catalogued dump without a ready cart artifact
- **WHEN** the user starts Build
- **THEN** a progress bar is visible while the build runs
- **THEN** the status/phase text changes at least once before completion (for example generating → compiling → linking)

#### Scenario: Build completion clears the in-progress bar
- **GIVEN** a Build that finishes successfully
- **WHEN** the artifact becomes ready
- **THEN** the in-progress progress bar is no longer shown as an active build
- **THEN** the UI reflects AOT ready (Play available per existing gates)
