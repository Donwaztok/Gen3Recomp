## 1. CLI

- [ ] 1.1 Parse `--rom`, `--bios`, `--help`, and `--version` in `app`
- [ ] 1.2 Implement usage text and exit codes `0` / `2` / `1`
- [ ] 1.3 Fail on missing files with human-readable errors via diagnostics

## 2. Optional picker

- [ ] 2.1 Either implement one native file picker for omitted `--rom`, or require `--rom` and document the choice in README
- [ ] 2.2 Ensure cancel / omission does not crash

## 3. Tests and docs

- [ ] 3.1 Add Catch2 tests for help, version, unknown flag, and missing ROM path
- [ ] 3.2 On success in this milestone, print resolved paths and exit 0
- [ ] 3.3 Update README usage examples
