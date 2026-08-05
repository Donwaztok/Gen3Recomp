#!/usr/bin/env bash
# Generate cart AOT C++ for a catalogued dump (local / gitignored output).
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
rom=""
config_override=""
out="$root/generated/rom"

usage() {
  echo "usage: $0 [--config data/<title>_usa.toml] [/path/to/game.gba]" >&2
  echo "  Default ROM search: ./roms/*.gba then ./Pokemon - Emerald*.gba" >&2
  exit 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --config)
      [[ $# -ge 2 ]] || usage
      config_override="$2"
      shift 2
      ;;
    -h|--help) usage ;;
    *)
      rom="$1"
      shift
      ;;
  esac
done

if [[ -z "$rom" ]]; then
  if [[ -f "$root/roms/Pokemon - Emerald Version (USA, Europe).gba" ]]; then
    rom="$root/roms/Pokemon - Emerald Version (USA, Europe).gba"
  elif [[ -f "$root/Pokemon - Emerald Version (USA, Europe).gba" ]]; then
    rom="$root/Pokemon - Emerald Version (USA, Europe).gba"
  else
    usage
  fi
fi

[[ -f "$rom" ]] || usage

sha1="$(sha1sum "$rom" | awk '{print $1}')"
config="${config_override:-}"
if [[ -z "$config" ]]; then
  case "$sha1" in
    f3ae088181bf583e55daf962a92bb46f4f1d07b7) config="$root/data/emerald_usa.toml" ;;
    f28b6ffc97847e94a6c21a63cacf633ee5c8df1e) config="$root/data/ruby_usa.toml" ;;
    3ccbbd45f8553c36463f13b938e833f652b793e4) config="$root/data/sapphire_usa.toml" ;;
    4722efb8cd45772ca32555b98fd3b9719f8e60a9) config="$root/data/sapphire_usa_rev1.toml" ;;
    *)
      echo "warning: SHA-1 $sha1 not in MVP catalog configs; using emerald_usa.toml as template" >&2
      config="$root/data/emerald_usa.toml"
      ;;
  esac
fi

recompile="$root/build/_gbarecomp/gba_recompile"
symbols="$out/symbols.tsv"

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
echo "SHA-1=$sha1 config=$config"
echo "Dev link: cmake -S \"$root\" -B \"$root/build\" && cmake --build \"$root/build\" -j\$(nproc) --target gen3recomp"
echo "Or one-shot artifact: ./scripts/build_cart_artifact.sh \"$rom\""
