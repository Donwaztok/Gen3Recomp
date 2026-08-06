## Purpose

Fetches and caches per-title cover images for the launcher from curated public URLs without distributing Nintendo assets in git or Releases.

## ADDED Requirements

### Requirement: Curated cover sources per catalog title
Each MVP catalog title MUST have a documented cover source preference (Wikimedia Commons / Wikipedia file URL preferred; Cover Project URL allowed as fallback). The application MUST NOT scrape arbitrary search pages at runtime.

#### Scenario: Emerald has a configured source
- **GIVEN** the builtin catalog entry for Emerald USA
- **WHEN** the cover resolver runs
- **THEN** it uses the curated URL mapping for that title (or catalog id / SHA-1)

### Requirement: On-demand fetch into user-data cache
Cover bytes MUST be stored under user data (not the repo). A successful fetch MUST be reused on later launches without re-downloading unless the cache entry is missing or invalidated.

#### Scenario: Second launch uses cache
- **GIVEN** a cover was previously fetched for Emerald
- **WHEN** the launcher opens again online or offline
- **THEN** the cached image is used without requiring a network round-trip when the cache file is present

### Requirement: No covers in Releases or git
Distributed project artifacts and the git tree MUST NOT contain Nintendo cover image payloads. Optional local override files under a user-writable path MAY be supported and MUST remain gitignored.

#### Scenario: Clean clone has no cover blobs
- **WHEN** a contributor clones the repository without user-data
- **THEN** no catalog cover PNGs/JPEGs are present in tracked files

### Requirement: Fetch failures degrade gracefully
Network errors, HTTP failures, or unsupported image data MUST result in a placeholder and a diagnostic log; they MUST NOT crash the launcher or clear the ROM list.

#### Scenario: Cover Project unreachable
- **GIVEN** the preferred URL fails
- **WHEN** the resolver attempts fetch
- **THEN** the UI shows a placeholder and may try a documented fallback URL once before giving up for that session
