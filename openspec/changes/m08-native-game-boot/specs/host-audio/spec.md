## Purpose

Plays backend audio on the host without implementing a Pokémon sound engine.

## ADDED Requirements

### Requirement: PCM playback
The audio module MUST submit PCM samples produced by the session backend to an SDL3 audio device.

#### Scenario: Audio device available
- **GIVEN** a running native session and a working host audio device
- **WHEN** the backend emits samples
- **THEN** those samples are queued to the host device

### Requirement: Audio init failure is loud
If the audio device cannot be opened, the application MUST log a clear error. It MAY continue muted. It MUST NOT crash.

#### Scenario: Missing audio device
- **WHEN** host audio initialization fails
- **THEN** a diagnostic error is logged and the session can continue without sound
