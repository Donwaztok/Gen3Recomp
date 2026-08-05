## Why

Boot will be messy if session lifecycle is invented inside the gba-recomp adapter. A small runtime session with a null backend lets the host loop exist independently of upstream.

## What Changes

- Add a `runtime` session: start, tick, stop.
- Add a null/stub backend that produces placeholder frames (and later silence).
- Move the M05 loop into runtime so App only starts a session.
- Prepare host service hooks for video (required), input/audio (stubs ok).

## Capabilities

### New Capabilities
- `runtime-session`: Host-owned session lifecycle and tick loop, backend-agnostic.

### Modified Capabilities
- (none)

## Impact

- `app` becomes “parse, validate, start session”.
- Enables M07 to plug a real backend without rewriting the loop.
