## ADDED Requirements

### Requirement: Runtime cart artifact activation for players
For the player UX, the provider MUST be able to use a ready user-data cart artifact for a catalogued SHA-1 without requiring the user to reconfigure CMake or relink the `gen3recomp` executable.

#### Scenario: Artifact ready then play
- **GIVEN** a valid cart artifact under user data for the dump’s SHA-1
- **WHEN** prepare runs from the launcher Play action on a stock host binary
- **THEN** static cart coverage is active for that session without a developer rebuild step

### Requirement: Provider can build or report missing artifact
Prepare or a dedicated provider operation MUST report whether the cart artifact is missing and MUST support triggering or integrating with the documented local build so the launcher AOT gate can complete.

#### Scenario: Missing artifact is observable
- **GIVEN** no cart artifact for the selected SHA-1
- **WHEN** the launcher queries readiness
- **THEN** the provider/host reports that a build is required

### Requirement: Enabled mods passed into prepare
Prepare MUST accept an optional list of enabled mod package identities or paths from the host so the adapter can apply supported modifications. The provider MUST NOT load arbitrary third-party recompiler plugins (D6).

#### Scenario: No plugin provider scan
- **WHEN** prepare runs with mods enabled
- **THEN** only the compile-time gba adapter loads execution code
- **THEN** mods are inputs to that adapter, not alternate provider binaries
