## ADDED Requirements

### Requirement: Cart artifact script may emit machine-readable progress
The documented cart artifact build script MAY emit stable progress lines (for example `PROGRESS current=… total=… phase=…`) on stdout so the launcher can drive a determinate progress bar. Free-text human logs MAY remain; progress lines MUST NOT break non-launcher CLI use of the script.

#### Scenario: Script progress line is greppable
- **WHEN** a maintainer runs `scripts/build_cart_artifact.sh` on a catalogued ROM
- **THEN** stdout includes human-readable status as today
- **THEN** if progress lines are implemented, they follow the documented `PROGRESS` prefix (or equivalent) without requiring a TTY
