## 1. Pin upstream

- [ ] 1.1 Add gba-recomp as a pinned third-party dependency
- [ ] 1.2 Write `third_party/README.md` with revision, license, and upgrade notes
- [ ] 1.3 Attribute gba-recomp in third-party docs and confirm the root LICENSE remains PolyForm Noncommercial

## 2. Seam and adapter

- [ ] 2.1 Add `recomp` provider/session contracts used by runtime
- [ ] 2.2 Implement `src/recomp/gba/` adapter only in that folder
- [ ] 2.3 Prepare from validated ROM + BIOS + Game Definition
- [ ] 2.4 Direct caches/generated output to gitignored user-data or build paths

## 3. Wiring and tests

- [ ] 3.1 Runtime default backend becomes the gba adapter after successful prepare
- [ ] 3.2 Add seam tests with a fake provider
- [ ] 3.3 Add an optional env-gated integration test for local dumps
- [ ] 3.4 Grep/check that non-adapter modules do not include upstream headers
