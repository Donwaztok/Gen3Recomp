## Why

Primary Play and Build still live in the footer, away from the cover the player is looking at. Putting an icon-only Play (and Build when AOT is missing) on the cover makes the next action obvious without hunting the footer.

## What Changes

- Overlay each cover tile with an **icon-only Play** control when cart AOT is ready (subject to existing BIOS/host gates).
- Overlay an **icon-only Build** control when cart AOT is not ready, so the one-time compile is started from the cover.
- Keep mouse selection of the tile; action icons MUST run Play/Build for that tile without requiring a separate footer click (footer actions MAY remain as secondary).
- Icons MUST be recognizable (play triangle / build-or-wrench-or-hammer) with accessible labels (`aria-label` / tooltip), even when no text is shown on the button face.

### Non-goals

- Removing the footer entirely
- Redesigning the whole library chrome or dark theme
- Changing AOT/BIOS gate rules
- Auto-Play on cover click without an explicit Play control

### Assumption

“Na frente do cover” means an overlay centered (or bottom-centered) on the cover image, not a separate column beside the tile. Footer Play/Build remain available as a backup.

## Capabilities

### New Capabilities

_(none)_

### Modified Capabilities

- `launcher-ui`: Cover tiles MUST expose icon-only Play when AOT-ready (gates permitting) and icon-only Build when AOT is missing; actions apply to that tile’s dump.

## Impact

- Frontend: `launcher/src/App.tsx`, `launcher/src/index.css` (overlay positioning, hit targets)
- Possibly HeroUI `Button` icon-only / `isIconOnly` patterns
- Same invoke paths as footer Play/Build (`play_rom`, `build_cart`)

Milestone: Tauri launcher UX polish (cover-primary actions).
