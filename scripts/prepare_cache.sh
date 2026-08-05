#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
rom="${1:-$root/Pokemon - Emerald Version (USA, Europe).gba}"
bios="${2:-$root/gba_bios.bin}"
frames="${3:-3600}"

if [[ ! -f "$rom" || ! -f "$bios" ]]; then
  echo "usage: $0 /path/to/game.gba /path/to/gba_bios.bin [frames]" >&2
  exit 1
fi

exec "$root/build/gen3recomp" --rom "$rom" --bios "$bios" --prepare "$frames"
