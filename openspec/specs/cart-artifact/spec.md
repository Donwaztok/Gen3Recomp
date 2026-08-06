# cart-artifact Specification

## Purpose
Defines a durable, SHA-1-keyed native cart artifact in user data so prepare can reuse full-cart AOT across host rebuilds without committing ROM-derived code or forcing a full relink of gen3recomp for every dump.
## Requirements
### Requirement: Artifact keyed by ROM identity
After successful local cart AOT for a catalogued dump, the system MUST be able to store a reusable native cart artifact under the user’s data directory keyed by the ROM SHA-1 (alongside existing save and self-heal cache layouts).

#### Scenario: Second host build reuses artifact
- **GIVEN** a previously built cart artifact for a catalogued SHA-1
- **WHEN** the user rebuilds only the host and launches the same dump
- **THEN** prepare prefers that artifact over regenerating and recompiling the entire cart corpus from scratch

### Requirement: Missing artifact triggers one-time generation
When no valid cart artifact exists for a catalogued dump, prepare MUST either use a developer-linked local corpus if present, or guide/run generation of the artifact. It MUST NOT silently claim full-speed AOT coverage with an empty cart dispatch.

#### Scenario: Empty dispatch without artifact
- **GIVEN** no linked corpus and no user-data cart artifact
- **WHEN** the user launches a catalogued dump
- **THEN** the application still boots if self-heal is available, but diagnostics or docs make clear that static AOT is missing
- **THEN** the preferred remediation is generating the cart artifact or local corpus—not `--prepare` alone

### Requirement: Artifact is not a plugin ABI
Cart artifacts MUST be loaded only by the gba-recomp adapter inside the recompiler provider. The application MUST NOT scan arbitrary plugin directories or load third-party provider binaries.

#### Scenario: No external provider discovery
- **WHEN** the application starts
- **THEN** it does not discover recompiler providers from user-supplied plugin folders

### Requirement: Artifact stays off the source tree
Cart artifacts MUST live under user data (or other gitignored machine-local paths), never as committed files in the repository.

#### Scenario: Identity path
- **WHEN** an artifact is written for ROM SHA-1 `S`
- **THEN** its path is under the gen3recomp user-data root and includes `S` (or an equivalent stable identity segment)

