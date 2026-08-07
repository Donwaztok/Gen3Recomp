## Context

See proposal.md — Why. The Tauri library already lists covers with footer Play/Build. User wants the primary action on the cover as icon-only: Play when AOT-ready, Build when not.

Constraints: keep AOT/BIOS gates; HeroUI + existing dark shell; no Nintendo art in repo.

## Goals / Non-Goals

**Goals:**
- Centered (or bottom-center) overlay action on each cover
- Icon-only Play vs Build based on `aot_ready`
- `stopPropagation` so icon click does not only select
- Accessible labels; footer actions remain secondary

**Non-Goals:**
- Removing footer
- Auto-play on bare cover click
- Changing gate logic

## Decisions

### D1 — Which icon when
- **Choice:** If `!aot_ready` → Build icon. If `aot_ready` → Play icon (disabled/hidden style when `!canPlay` for that tile: missing BIOS/host).
- **Why:** Matches user ask; one primary action per tile.

### D2 — Layout
- **Choice:** Absolute overlay centered on `.g3-cover`; dim scrim optional on hover/focus for contrast.
- **Why:** “Na frente do cover”; keeps title meta below.

### D3 — Markup
- **Choice:** Cover remains a select target; action is a nested `<button type="button">` (or HeroUI icon Button) with `onClick`/`onPress` calling `onPlay`/`onBuild` for that `rom`, stopping propagation. Avoid nesting interactive controls incorrectly: prefer tile as `<div>` with role/button for select OR keep tile button and use `pointer-events` carefully — prefer restructuring tile to a non-button container with select-on-click and separate action button (valid HTML).
- **Why:** Nested `<button>` inside `<button>` is invalid; apply must fix structure.

### D4 — Icons
- **Choice:** Inline SVG (play triangle; hammer/wrench or download-build) — no new npm icon pack required unless HeroUI already exposes icons.
- **Why:** Keep deps minimal.

## Risks / Trade-offs

- [Touch targets too small] → Min ~40×40 CSS px hit area
- [Footer vs cover confusion] → Keep footer; cover is primary
- [Busy state] → Disable cover actions while `busy`

## Migration Plan

1. Restructure tile markup (no nested buttons).
2. Add overlay CSS + icons.
3. Wire per-tile Play/Build to existing invokes.
4. Smoke in launcher UI.
