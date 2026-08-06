## ADDED Requirements

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

## MODIFIED Requirements

### Requirement: No ROM-derived sources in git
Preparation MUST write any generated or cache artifacts outside the source tree (user data or build/work directories that are gitignored). Developer-local cart AOT under a gitignored `generated/` path is allowed. Public releases and the git repository MUST NOT contain ROM-derived generated C++.

#### Scenario: Clean tree after prepare
- **WHEN** provider prepare runs on a developer machine
- **THEN** no ROM-derived generated sources are created as version-controlled files

#### Scenario: Local generated corpus is optional
- **WHEN** a clean clone builds without `generated/rom/`
- **THEN** the host still links (stub dispatch) and does not fail configure solely because cart AOT is absent
