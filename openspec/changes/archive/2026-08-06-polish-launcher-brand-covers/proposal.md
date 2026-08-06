## Why

The Tauri launcher still shows lowercase `gen3recomp` in a serif brand font, and Ruby/Sapphire tiles crop the shared dual-box Wikipedia image through the spine — Emerald already looks correct. Players need a coherent product title (**Gen3Recomp**), a clearer brand typeface, and covers that read as front-box art for every MVP title.

## What Changes

- Present the player-facing product name as **Gen3Recomp** in the launcher hero, HTML document title, and Tauri window title.
- Replace the current brand serif (Fraunces) with a modern sans display font suited to the dark charcoal/green shell.
- Improve MVP cover presentation so Ruby and Sapphire tiles show a clean front cover (not mid-spine crop of the dual Ruby/Sapphire sheet). Prefer CSS per-title crop of the curated dual image; allow curated URL upgrades only when D7-safe (no shipping cover blobs).
- Keep cover bytes in user-data cache only; no Nintendo art in git or Releases.

### Non-goals

- New catalog titles beyond MVP USA Ruby / Sapphire / Emerald
- Redesigning the whole launcher layout, HeroUI actions, or dark theme system
- Shipping cover images in git or Releases
- Renaming the C++ host binary, crate ids, or filesystem data dirs (`gen3recomp`)
- Changing cover-fetch legal policy (curated URLs only; D7)

## Capabilities

### New Capabilities

_(none)_

### Modified Capabilities

- `launcher-ui`: Brand string and window/document title MUST read **Gen3Recomp**; brand typography MUST use the chosen modern sans display face (not Fraunces).
- `cover-art`: Cover tiles for Ruby and Sapphire MUST present a front-cover crop (or equivalent per-title art) so the dual-box source does not show spine/GBA side as the tile face; Emerald keeps a clean front-cover presentation.

## Impact

- Frontend: `launcher/src/App.tsx`, `launcher/src/index.css`, `launcher/index.html`
- Tauri config: `launcher/src-tauri/tauri.conf.json` window/product display title
- Optional: `launcher/src-tauri/src/catalog.rs` / `data/covers.toml` if curated URLs change; users may need cover-cache clear after URL changes
- Legal: D7 unchanged — covers stay user-local cache; only URL strings / CSS in repo
- Assumption: CSS `object-position` / scale per `game_id` on the existing dual-box URL is enough for Ruby/Sapphire unless apply finds a better D7-safe Wikimedia front cover

Milestone: post-Tauri launcher UX polish (player shell branding + cover readability).
