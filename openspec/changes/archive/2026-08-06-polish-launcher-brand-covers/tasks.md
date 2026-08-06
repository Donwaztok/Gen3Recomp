## 1. Branding and typography

- [x] 1.1 Set hero brand text to `Gen3Recomp` (optional accent on `Recomp`) in `launcher/src/App.tsx`
- [x] 1.2 Set HTML document title and Tauri window title to `Gen3Recomp` (`launcher/index.html`, `tauri.conf.json`); keep crate/binary/data-dir ids as `gen3recomp` unless `productName` alone is required for packaging
- [x] 1.3 Replace Fraunces with Sora (brand) + Outfit (body) in `launcher/src/index.css`; remove Fraunces from imports and brand/placeholder rules

## 2. Cover presentation

- [x] 2.1 Add per-`game_id` CSS class on cover tiles (e.g. `g3-cover--ruby-usa`) in `App.tsx`
- [x] 2.2 Add CSS crops for Ruby / Sapphire dual-box (left/right front panels) and a light Emerald front tweak so tiles are not mid-spine
- [x] 2.3 Only if CSS crop is insufficient: update curated URLs in `catalog.rs` / `data/covers.toml` with D7-safe Wikimedia fronts; document clearing `~/.local/share/gen3recomp/covers/` when URLs change

## 3. Validation

- [x] 3.1 Rebuild the Tauri launcher (`npm run tauri:build` with local `CARGO_TARGET_DIR`)
- [x] 3.2 Manual smoke via `./scripts/run_launcher.sh`: brand Gen3Recomp + new font; Ruby/Sapphire/Emerald covers look like front boxes; Play/Build unchanged
- [x] 3.3 Confirm no cover image blobs added to git; D7 still holds
