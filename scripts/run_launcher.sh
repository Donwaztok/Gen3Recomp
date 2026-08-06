#!/usr/bin/env bash
# Run gen3recomp-launcher with env that works on Hyprland + NVIDIA.
# Prefer a production Tauri build (`npm run tauri:build`). Bare `cargo build`
# often leaves the WebView pointed at http://localhost:1420 (Connection refused).
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
bin="${GEN3RECOMP_LAUNCHER:-}"
if [[ -z "$bin" ]]; then
  for candidate in \
    "$root/launcher/src-tauri/target/release/gen3recomp-launcher" \
    "$root/dist/release/"*/bin/gen3recomp-launcher \
    "$(command -v gen3recomp-launcher 2>/dev/null || true)"
  do
    if [[ -n "$candidate" && -x "$candidate" ]]; then
      bin="$candidate"
      break
    fi
  done
fi
if [[ -z "${bin:-}" || ! -x "$bin" ]]; then
  echo "error: gen3recomp-launcher not found." >&2
  echo "Build production UI: cd launcher && npm run tauri:build" >&2
  echo "(Do not use cargo build alone — that causes 'localhost Connection refused'.)" >&2
  exit 1
fi

export GDK_BACKEND="${GDK_BACKEND:-x11}"
export WEBKIT_DISABLE_COMPOSITING_MODE="${WEBKIT_DISABLE_COMPOSITING_MODE:-1}"

exec "$bin" "$@"
