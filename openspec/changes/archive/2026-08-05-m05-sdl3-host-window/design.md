## Context

Validation works. This milestone creates the host surface used by M06–M08.

## Goals / Non-Goals

**Goals:**
- SDL3 init, window, event poll, shutdown in `platform`.
- `video` uploads a placeholder.
- Automated test that platform init/shutdown works in a headless-friendly way if possible; otherwise a narrow unit test plus a documented manual check.
- Window title like `gen3recomp` or `gen3recomp — <display name>` is acceptable if the title string is supplied by `app`/`runtime`, not looked up inside `platform`.

**Non-Goals:**
- Scaling filters, widescreen, ImGui, multiple windows, Vulkan.
- Audio/input mapping (M08 / optional tiny quit-key here).
- Game backend.

## Decisions

- **Fixed logical size 240×160** for the placeholder, integer-scaled to a comfortable window (e.g. 3× or 4×). Scaling math stays in `video` or `platform`, not in game code.
- **No renderer abstraction pyramid.** One SDL3 implementation.
- **App still orchestrates:** validate → create platform → present placeholder loop → quit.
- **Escape or Q may quit** as a convenience, implemented via platform events mapped later by input; in M05 raw SDL key quit is acceptable inside the app loop, but do not invent a full input module yet unless it is smaller than special-casing.

## Risks / Trade-offs

- [CI without a display] → Guard GUI tests; keep a non-GUI unit test for any pure scaling math.
- [SDL3 install friction] → Document distro packages / vendoring choice in README.

## Migration Plan

README: how to install SDL3 and what the placeholder window means.

## Open Questions

None blocking.
