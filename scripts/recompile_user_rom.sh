#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
rom="${1:-$root/Pokemon - Emerald Version (USA, Europe).gba}"
out="$root/generated/rom"
recompile="$root/build/_gbarecomp/gba_recompile"
config="${GEN3RECOMP_ROM_CONFIG:-$root/data/emerald_usa.toml}"
symbols="$out/symbols.tsv"

if [[ ! -f "$rom" ]]; then
  echo "usage: $0 /path/to/game.gba" >&2
  exit 1
fi
if [[ ! -x "$recompile" ]]; then
  cmake --build "$root/build" --target gba_recompile
fi

mkdir -p "$out"
rm -f "$out"/recompiled_*.cpp "$out"/dispatch_table.cpp "$out"/recompiled.h "$out"/symbol_map.cpp
python3 "$root/scripts/scan_rom_symbols.py" "$rom" -o "$symbols"
cmd=("$recompile" --rom "$rom" --max-functions 250000 --out "$out" --symbols "$symbols")
if [[ -f "$config" ]]; then
  cmd+=(--config "$config")
else
  cmd+=(--entry 0x080000C0)
fi
"${cmd[@]}"
echo "Cart recomp written to $out (local only; do not commit)."
echo "Reconfigure and rebuild: cmake -S \"$root\" -B \"$root/build\" && cmake --build \"$root/build\" -j\$(nproc)"
