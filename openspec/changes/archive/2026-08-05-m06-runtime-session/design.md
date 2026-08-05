## Context

A window loop exists in app/platform. This milestone extracts a session and introduces the backend seam that M07 will implement for real.

## Goals / Non-Goals

**Goals:**
- Small session object/functions in `runtime`.
- Null backend in runtime or app test doubles.
- Catch2 test: start null session, request stop, join/return.
- Pass Game Definition through without inspecting title ids.

**Non-Goals:**
- Real recompilation.
- Timing accuracy / audio sync.
- Threaded renderer unless SDL requires it; prefer single-threaded loop.

## Decisions

- **Single-threaded tick loop** for MVP: poll events → backend.step → video.present → repeat.
- **Backend contract described in prose / tiny abstract type:** start(config), step(host_input) -> frame (+ optional audio later), stop(). Do not design a 20-method interface.
- **Null backend** returns a static test pattern each step.
- **Timing:** simple SDL delay or vsync; good enough. No dedicated scheduler module.

## Risks / Trade-offs

- [Over-abstracting the backend] → Keep the contract tiny; M07 may adjust names when wrapping gba-recomp.
- [Putting null backend in the product permanently] → Fine as a debug flag later; in M06 it is the only backend.

## Migration Plan

None beyond moving loop code from app into runtime.

## Open Questions

None blocking. Exact function names wait for implementation.
