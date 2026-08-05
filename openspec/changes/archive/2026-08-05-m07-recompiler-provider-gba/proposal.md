## Why

gba-recomp is the first execution engine, but it must not colonize the host. This milestone isolates upstream behind the provider seam and proves a session artifact can be prepared from a validated dump.

## What Changes

- Add `recomp` module and `src/recomp/gba/` adapter.
- Pin gba-recomp as a third-party dependency.
- Implement provider prepare + session backend wrapper.
- Keep upstream types inside the adapter folder.
- Preparing may still use interpreter/JIT rather than shipping generated C++.
- Full title-screen boot acceptance waits for M08; M07 must at least prepare and start a backend session without leaking types.

## Capabilities

### New Capabilities
- `recompiler-provider`: Isolated provider seam with a gba-recomp adapter that prepares execution from user dumps.

### Modified Capabilities
- (none)

## Impact

- Heaviest dependency enters the tree.
- Upstream attribution is recorded next to the pin; license remains PolyForm NC.
- Runtime can select the gba backend instead of null when prepare succeeds.
