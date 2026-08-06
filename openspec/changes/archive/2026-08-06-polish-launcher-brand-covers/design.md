## Context

See proposal.md — Why. The Tauri launcher already loads curated Wikimedia covers into user-data and renders a dark HeroUI shell. Brand still uses lowercase `gen3recomp` + Fraunces; Ruby/Sapphire share `PokemonRubySapphireBox.jpg` and `object-fit: cover` centers on the spine.

Constraints: D7 (no cover blobs in git/Releases), curated URLs only, MVP USA titles only, keep host binary / data-dir ids as `gen3recomp`.

## Goals / Non-Goals

**Goals:**
- Visible brand and window/document title: **Gen3Recomp**
- Sans display brand font (e.g. Sora or Outfit) replacing Fraunces
- Per-`game_id` CSS crop (and optional URL tweak) so Ruby/Sapphire/Emerald tiles read as front covers

**Non-Goals:**
- Renaming crates, Cargo package name, or XDG data directory
- Full visual redesign beyond brand + cover crop
- Baking cover images into the repo

## Decisions

### D1 — Display name vs identifiers
- **Choice:** UI/window/`productName` show `Gen3Recomp`; keep filesystem ids (`gen3recomp`, `com.gen3recomp.launcher`, binary `gen3recomp-launcher`) unchanged unless packaging forces `productName` only.
- **Why:** Brand polish without breaking paths, cache dirs, or scripts.
- **Alt:** Rename everything to Gen3Recomp — rejected (migration noise, out of scope).

### D2 — Brand typeface
- **Choice:** Google Fonts **Sora** (brand) + **Outfit** (UI body), drop Fraunces.
- **Why:** Modern sans fits charcoal/green shell; matches earlier polish intent.
- **Alt:** Keep Fraunces with weight tweak — rejected (user asked to change the font).

### D3 — Cover crop strategy
- **Choice:** Keep dual-box Wikimedia URL for Ruby/Sapphire; add CSS modifiers per `game_id` (`object-position` + slight scale) so each tile crops its half. Emerald keeps PT Wikipedia single cover; minor position tweak allowed.
- **Why:** No new legal surface; works offline once cached; fixes the mid-spine crop without new assets.
- **Alt:** Separate Wikimedia/IGDB URLs per title — allowed later if apply finds stable D7-safe fronts; CSS first.
- **Implementation note:** Attach `g3-cover--{game_id}` on the cover element; sanitize id for CSS class safety.

### D4 — Cache invalidation
- **Choice:** If URLs unchanged, no cache clear needed (CSS-only). If URLs change, document deleting `~/.local/share/gen3recomp/covers/` (or platform equivalent).
- **Why:** Avoid surprising re-downloads when unnecessary.

## Risks / Trade-offs

- [CSS crop fragile across aspect ratios] → Tune positions against 3:4 tiles; verify Ruby/Sapphire/Emerald on Linux build.
- [`productName` change affects installer folder names] → Prefer window/HTML title + hero; set `productName` to Gen3Recomp only if Tauri packaging stays acceptable.
- [Font CDN offline] → System sans fallbacks in `font-family` stack.

## Migration Plan

1. Update brand strings and fonts in frontend + Tauri window title.
2. Add per-title cover CSS classes.
3. Rebuild launcher (`npm run tauri:build`); smoke via `./scripts/run_launcher.sh`.
4. If cover URLs change, clear user cover cache and re-fetch.
