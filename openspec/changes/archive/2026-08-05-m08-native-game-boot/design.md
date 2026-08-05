## Context

Provider seam exists. This milestone is acceptance of native boot and host I/O completeness.

## Goals / Non-Goals

**Goals:**
- Title screen on Emerald USA first, then Ruby and Sapphire USA.
- Keyboard input + audio path.
- Cartridge save files in user data.
- Manual boot checklist checked into docs (`docs/manual-boot.md` or README section).

**Non-Goals:**
- Full playthrough QA.
- Link cable, multiplayer, mods, widescreen, save states.
- Perfect audio sync.

## Decisions

- **Emerald first** because it is the common reference and upstream’s most visible Gen3 target; then R/S using the same path.
- **Controls:** arrows / Z / X / Enter / Backspace / A / S (or another boring layout). Document in README. Match upstream only if it reduces adapter friction.
- **Audio:** SDL3 audio stream; if upstream already outputs via SDL internally, the adapter may own audio instead of `audio/` — but only if that keeps isolation. Prefer routing samples out of the adapter into `audio` so Platform/SDL audio stays host-side. If upstream tightly couples SDL audio, wrapping the whole host audio inside the adapter is acceptable temporarily; record a follow-up to unwrap rather than spreading SDL calls.
- **Saves:** one file per identity in the user data dir. Let upstream save-chip code do the format.

## Risks / Trade-offs

- [Title screen is subjective to automate] → Manual checklist is the acceptance test; optional screenshot later.
- [R/S flash/RTC quirks] → Stay on upstream behavior; fix only via adapter hints in Game Definitions.
- [Scope creep into gameplay polish] → Stop at title screen + basic input/audio/save.

## Migration Plan

README becomes a real quick-start: dump requirements, BIOS, controls, save location.

## Open Questions

Resolved at apply: upstream pin `2952aff` still only exposes blocking `gbarecomp::run_game` with SDL2 `HostWindow` audio/input. Native boot wraps that call inside `src/recomp/gba/`. SDL3 `audio`/`input` serve the null backend and the future steppable unwrap. Follow-up: pull PCM + KEYINPUT out of upstream without dual windows.
