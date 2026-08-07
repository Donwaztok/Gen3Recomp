## ADDED Requirements

### Requirement: Cart Build streams progress to the UI
The Tauri launcher backend MUST run the cart AOT build in a way that can report progress to the frontend while the process is still running (for example stdout line events or structured progress payloads). It MUST NOT be limited to returning only a single result string after the script exits when the UI needs live progress.

#### Scenario: Progress event during compile
- **GIVEN** a Build that compiles multiple cart shards
- **WHEN** the backend observes compile progress from the build script
- **THEN** the frontend receives one or more progress updates before the build command returns success or failure

#### Scenario: Failure still surfaces after streamed progress
- **GIVEN** a Build that fails mid-script
- **WHEN** the process exits non-zero
- **THEN** the launcher still reports the failure to the user (existing error path)
- **THEN** any in-progress bar is cleared or marked failed
