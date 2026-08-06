## ADDED Requirements

### Requirement: Build progress events identify the target dump
Cart AOT Build progress events emitted to the frontend MUST include a stable identity for the dump being built (SHA-1 of the catalogued ROM, or an equivalent id the UI already uses for tiles). The frontend MUST be able to bind progress to the correct cover without assuming a single global “building” selection.

#### Scenario: Progress payload includes dump identity
- **GIVEN** the user starts Build for a catalogued dump with a known SHA-1
- **WHEN** the backend emits a progress update during that Build
- **THEN** the payload includes that dump’s SHA-1 (or equivalent tile id)

### Requirement: Play command remains usable during Build
While a cart AOT Build is running, the Tauri launcher backend MUST still accept a Play request for a different dump that meets readiness gates. Build MUST NOT hold a process-wide lock that rejects unrelated Play invocations. Starting a second concurrent Build MAY be rejected with a clear error.

#### Scenario: Play succeeds while another Build runs
- **GIVEN** Build is running for dump A
- **WHEN** the frontend invokes Play for dump B with valid BIOS and ready AOT
- **THEN** the host session for dump B is spawned successfully
- **THEN** Build for dump A continues

#### Scenario: Concurrent second Build rejected
- **GIVEN** Build is already running for dump A
- **WHEN** the frontend invokes Build for dump B before A finishes
- **THEN** the backend does not start a second build process
- **THEN** the caller receives a clear error or busy indication
