# Risks

| ID | Risk | Impact | Likelihood | Mitigation |
|----|------|--------|------------|------------|
| R1 | gba-recomp APIs move quickly and break the adapter | Blocks M07–M08 | High | Pin a revision. Isolate all includes in `src/recomp/gba/`. Treat upstream upgrades as their own change. |
| R2 | Calling the project OSI “open source” despite PolyForm NC | Community trust | Low | D11: one PolyForm NC license; README says public noncommercial source. |
| R3 | Temptation to reimplement PPU/audio “cleanly” in gen3recomp | Months of duplicate work | Medium | D4. Video is a blit. Hardware stays upstream. |
| R4 | Title-specific hacks leak into Runtime to “just boot Emerald” | Destroys the Game Definition model | Medium | Spec + review rule: no title switches outside `game` catalog and provider hints. |
| R5 | Shipping or generating ROM-derived sources into git | Legal failure | Medium | gitignore generated output and ROM/BIOS paths. CI check for known dump hashes if practical. |
| R6 | Exact SHA-1 catalog is too strict for some dumps (overdumps, bad dumps, EU/JP) | User confusion | Medium | Clear error with computed SHA-1. Document supported dumps. Expand catalog deliberately later. |
| R7 | BIOS requirement surprises users | Support load | Medium | Document early. Validate with a dedicated error. Never silent HLE. |
| R8 | Interpreter/JIT path is slow or incomplete on cold boot | Poor first-run experience | Medium | Accept cold-start cost in MVP. Persist heal cache. Do not block MVP on full static coverage. |
| R9 | SDL3 packaging differences across Linux/Windows/macOS | Build friction | Medium | Document packages. Keep platform module thin. Defer polished installers. |
| R10 | Scope creep from upstream features (mods, widescreen, save states) | Missed MVP | High | future.md is the parking lot. Milestone rule 4. |
| R11 | Single maintainer + heavy generated code compile times | Slow iteration | Medium | Prefer runtime JIT/interpreter for user path; avoid multi-minute generated C++ in the inner loop. |
| R12 | License file missing at first code commit | Process | Low | M01 adds PolyForm Noncommercial `LICENSE` per D11. |

## Risk posture

Prefer **shipping a small honest host** over **matching every gba-recomp showcase feature**.
