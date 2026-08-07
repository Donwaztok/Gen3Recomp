## 1. Build script progress lines

- [x] 1.1 Emit stable `PROGRESS` (or equivalent) lines from `scripts/build_cart_artifact.sh` for generate/compile/link with current/total when known
- [x] 1.2 Keep existing human-readable `echo` output for CLI users

## 2. Tauri streaming

- [x] 2.1 Change `build_cart` to spawn the script and stream stdout lines (not only `Command::output()`)
- [x] 2.2 Emit `build-progress` (or documented event) payloads to the frontend during the run; return success/error when the process exits

## 3. Launcher UI

- [x] 3.1 Listen for progress events; show ProgressBar + phase label while Build runs (cover or footer)
- [x] 3.2 Clear/hide active progress on success or failure; keep existing error modal/status behavior

## 4. Validation

- [x] 4.1 Manual: start Build on a title needing AOT — bar/phase updates before completion
- [x] 4.2 Manual: successful Build unlocks Play; failed Build still shows an error
- [x] 4.3 CLI: `scripts/build_cart_artifact.sh` still works without the launcher
