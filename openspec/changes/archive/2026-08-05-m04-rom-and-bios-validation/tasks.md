## 1. Core hashing

- [x] 1.1 Implement SHA-1 in `core` (or finalize the M02 helper)
- [x] 1.2 Add vector tests for SHA-1

## 2. Catalog and identification

- [x] 2.1 Add a committed catalog data file for Ruby/Sapphire/Emerald USA with verified SHA-1s
- [x] 2.2 Implement Game Definition loading and lookup-by-SHA-1 in `game`
- [x] 2.3 Wire identification into `app` after path resolution
- [x] 2.4 Tests: match, mismatch, and catalog load — using temp files with synthetic catalog entries where possible

## 3. BIOS validation

- [x] 3.1 Add a BIOS hash catalog
- [x] 3.2 Require and validate BIOS after ROM identity succeeds
- [x] 3.3 Tests for missing / invalid / valid BIOS using synthetic catalogs

## 4. UX

- [x] 4.1 Print identified title on success and exit 0 in this milestone
- [x] 4.2 Document supported dumps, BIOS requirement, and error examples in README
