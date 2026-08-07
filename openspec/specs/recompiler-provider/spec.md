## Purpose

Isolates how native execution is prepared so gba-recomp can be used now and replaced later without rewriting the host.
## Requirements
### Requirement: Provider prepare uses Game Definition data
The recompiler provider MUST accept ROM bytes or path, validated BIOS input, and a Game Definition, then prepare an execution artifact or fail with a clear error.

#### Scenario: Prepare succeeds for a catalogued dump
- **GIVEN** a catalogued ROM, valid BIOS, and matching Game Definition
- **WHEN** the gba-recomp provider prepares execution
- **THEN** it returns a usable session backend or artifact and does not require title-specific logic in runtime

#### Scenario: Prepare failure is visible
- **WHEN** upstream preparation fails
- **THEN** the application reports a human-readable error and does not pretend a session is running

### Requirement: Adapter isolation
Only the gba adapter area MAY depend on gba-recomp. Host modules MUST depend on gen3recomp provider/session contracts.

#### Scenario: Leak check
- **WHEN** `src/app`, `src/runtime`, `src/platform`, `src/video`, `src/audio`, `src/input`, and `src/game` are inspected
- **THEN** they do not include gba-recomp headers

### Requirement: No ROM-derived sources in git
Preparation MUST write any generated or cache artifacts outside the source tree (user data or build/work directories that are gitignored). Developer-local cart AOT under a gitignored `generated/` path is allowed. Public releases and the git repository MUST NOT contain ROM-derived generated C++.

#### Scenario: Clean tree after prepare
- **WHEN** provider prepare runs on a developer machine
- **THEN** no ROM-derived generated sources are created as version-controlled files

#### Scenario: Local generated corpus is optional
- **WHEN** a clean clone builds without `generated/rom/`
- **THEN** the host still links (stub dispatch) and does not fail configure solely because cart AOT is absent

### Requirement: Compile-time provider wiring
The MVP MUST wire the gba-recomp provider at compile time. The application MUST NOT load arbitrary plugin binaries.

#### Scenario: No plugin scan
- **WHEN** the application starts
- **THEN** it does not discover recompiler providers from external plugin files

### Requirement: Prefer static cart coverage when available
When preparing a catalogued dump, the gba-recomp provider MUST prefer static cart coverage from a developer-linked local corpus or a valid user-data cart artifact over an empty cart dispatch that relies only on cold self-heal for ROM PCs.

#### Scenario: Static coverage engaged
- **GIVEN** a valid local cart corpus or cart artifact for the dump’s SHA-1
- **WHEN** the provider prepares the session
- **THEN** session startup reports or logs that static cart coverage is active
- **THEN** ROM execution does not depend on compiling every cart PC on first visit

### Requirement: IWRAM overlay policy remains explicit
Even with full-cart ROM AOT, preparation MUST keep an explicit policy for code copied into IWRAM (overlay heal and/or AOT `code_copy` configuration). The host MUST NOT assume ROM AOT alone covers RAM-resident guest PCs.

#### Scenario: RAM PCs are not permanently failed
- **GIVEN** Emerald USA with static cart AOT linked or loaded
- **WHEN** the game copies hot code into IWRAM during boot
- **THEN** those PCs remain executable via overlay heal and/or static overlay coverage rather than being marked permanently failed

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

