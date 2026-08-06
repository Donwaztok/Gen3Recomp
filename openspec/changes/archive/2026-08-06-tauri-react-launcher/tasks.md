## 1. Scaffold Tauri launcher

- [x] 1.1 Create `launcher/` with Tauri 2 + Vite React + TypeScript
- [x] 1.2 Add HeroUI v3 + Tailwind v4 (or documented fallback component library) and floating window defaults
- [x] 1.3 Wire npm/cargo scripts; document binary name (`gen3recomp-launcher` unless renamed at apply)

## 2. Tauri backend commands

- [x] 2.1 Implement ROM scan + catalog SHA-1 match against `data/` catalog
- [x] 2.2 Implement BIOS presence/status check (cwd / documented paths)
- [x] 2.3 Implement cover resolve: local override → user-data cache → curated fetch → placeholder
- [x] 2.4 Implement cart AOT ready check + Build action invoking existing host scripts
- [x] 2.5 Implement mods list/enable/disable via existing user-data layout
- [x] 2.6 Implement Play: spawn `gen3recomp --rom … --bios …` with path discovery

## 3. React library UI

- [x] 3.1 Cover grid + selection, BIOS/AOT badges, Build/Play/Add ROM/Mods actions
- [x] 3.2 Placeholders for missing covers; progress/errors for Build
- [x] 3.3 Mods panel; reject unknown dumps with clear messaging
- [x] 3.4 Apply brand-forward layout (compact floating window, one composition)

## 4. Host integration and product docs

- [x] 4.1 Default player path: Tauri entrypoint; bare `gen3recomp` without `--rom` exec-if-found or clear hint (per design)
- [x] 4.2 Demote SDL textured grid from README default; keep CLI `--rom`
- [x] 4.3 Update `openspec/product/decisions.md` (D3/D10) and README build/player sections (Linux + Win/macOS notes)
- [x] 4.4 Ensure Release/gitignore rules exclude covers, dumps, BIOS, cart artifacts (D7)

## 5. Validation

- [x] 5.1 Build launcher + host on Linux; smoke open library UI
- [x] 5.2 Manual: covers online/offline; Build then Play Emerald via launcher
- [x] 5.3 Manual: unknown ROM rejected; CLI `--rom` still works without Tauri
- [x] 5.4 Automated checks where cheap (catalog parse, path helpers, or frontend unit smoke)
