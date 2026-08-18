#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
bios="${1:-$root/gba_bios.bin}"
out="$root/generated/bios"
recompile="$root/build/_gbarecomp/gba_recompile"

if [[ ! -f "$bios" ]]; then
  echo "usage: $0 /path/to/gba_bios.bin" >&2
  exit 1
fi
if [[ ! -x "$recompile" ]]; then
  cmake --build "$root/build" --target gba_recompile
fi
mkdir -p "$out"
"$recompile" --bios "$bios" --out "$out"
echo "BIOS recomp written to $out (local only; do not commit)."
echo "Reconfigure and rebuild: cmake -S \"$root\" -B \"$root/build\" && cmake --build \"$root/build\""
