## 1. Cover metadata and cache

- [x] 1.1 Add curated cover URL map for USA Ruby / Sapphire / Emerald (Wikimedia primary, Cover Project optional fallback)
- [x] 1.2 Implement user-data cover cache path + load-from-cache / save-after-fetch
- [x] 1.3 Implement HTTPS fetch with timeout, background thread, graceful failure → placeholder
- [x] 1.4 Support optional local override `roms/covers/<id>.png` (document + gitignore)
- [x] 1.5 Ensure no cover image files are added to git or Release packaging (D7)

## 2. Floating grid launcher UI

- [x] 2.1 Replace debug-text list with floating compact SDL3 window (default size, not maximized)
- [x] 2.2 Render cover (or placeholder) grid with title + AOT readiness badge
- [x] 2.3 Mouse: click tile to select; click Play / Build / Add ROM / Mods; hover feedback
- [x] 2.4 Keep keyboard shortcuts as secondary
- [x] 2.5 Preserve BIOS status, AOT gate, progress/errors, mods panel behavior
- [x] 2.6 Spike ImGui vs custom textured UI; pick one and document in design notes if needed

## 3. Cross-platform host

- [x] 3.1 Make launcher/platform/cover code compile on Windows and macOS (fix Linux-only APIs)
- [x] 3.2 Extend `cart_artifact_abi_tag` / paths for Win and macOS (implement or stub with clear errors)
- [x] 3.3 Document build recipes for Windows, Linux, macOS in README (SDL3 + toolchain)
- [x] 3.4 Smoke-build or CI notes for at least one non-Linux host if a runner is available; otherwise checklist for manual verify

## 4. Validation

- [x] 4.1 Automated tests: cover cache hit skips network; missing cover yields placeholder path/API; bare launch still selects launcher mode
- [x] 4.2 Manual: online fetch shows Emerald/Ruby/Sapphire tiles; offline placeholders; mouse Play with ready AOT on Linux
- [x] 4.3 Manual: unknown ROM still rejected; CLI `--rom` unchanged
