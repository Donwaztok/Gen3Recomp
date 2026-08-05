#!/usr/bin/env bash
# Build a durable libcart.so under ~/.local/share/gen3recomp/cart_aot/<sha1>/abi3-linux-x64/
# so host rebuilds only need to relink against the .so (no recompile of ~800MiB C++).
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
rom="${1:-}"
jobs="${GEN3RECOMP_CART_JOBS:-8}"
abi="abi3-linux-x64"

if [[ -z "$rom" ]]; then
  echo "usage: $0 /path/to/game.gba" >&2
  exit 1
fi
if [[ ! -f "$rom" ]]; then
  echo "error: ROM not found: $rom" >&2
  exit 1
fi

sha1="$(sha1sum "$rom" | awk '{print $1}')"
xdg="${XDG_DATA_HOME:-$HOME/.local/share}"
artifact_dir="$xdg/gen3recomp/cart_aot/$sha1/$abi"
src_dir="$artifact_dir/src"
lib="$artifact_dir/libcart.so"

echo "==> cart artifact sha1=$sha1 abi=$abi"
mkdir -p "$src_dir"

# Generate sources into the artifact tree (not the repo) so CI clones stay clean.
GEN3RECOMP_ROM_CONFIG="${GEN3RECOMP_ROM_CONFIG:-}" \
  bash -c '
    set -euo pipefail
    root="'"$root"'"
    rom="'"$rom"'"
    out="'"$src_dir"'"
    export GEN3_FORCE_OUT="$out"
    # Inline the recompile steps with a custom out dir.
    recompile="$root/build/_gbarecomp/gba_recompile"
    [[ -x "$recompile" ]] || cmake --build "$root/build" --target gba_recompile
    sha1="$(sha1sum "$rom" | awk "{print \$1}")"
    config=""
    case "$sha1" in
      f3ae088181bf583e55daf962a92bb46f4f1d07b7) config="$root/data/emerald_usa.toml" ;;
      f28b6ffc97847e94a6c21a63cacf633ee5c8df1e) config="$root/data/ruby_usa.toml" ;;
      3ccbbd45f8553c36463f13b938e833f652b793e4) config="$root/data/sapphire_usa.toml" ;;
      4722efb8cd45772ca32555b98fd3b9719f8e60a9) config="$root/data/sapphire_usa_rev1.toml" ;;
      *) config="$root/data/emerald_usa.toml" ;;
    esac
    [[ -n "${GEN3RECOMP_ROM_CONFIG:-}" ]] && config="$GEN3RECOMP_ROM_CONFIG"
    mkdir -p "$out"
    rm -f "$out"/recompiled_*.cpp "$out"/dispatch_table.cpp "$out"/recompiled.h "$out"/symbol_map.cpp
    python3 "$root/scripts/scan_rom_symbols.py" "$rom" -o "$out/symbols.tsv"
    cmd=("$recompile" --rom "$rom" --max-functions 250000 --out "$out" --symbols "$out/symbols.tsv")
    if [[ -f "$config" ]]; then cmd+=(--config "$config"); else cmd+=(--entry 0x080000C0); fi
    "${cmd[@]}"
  '

gbarecomp_inc=(
  -I"$root/third_party/gbarecomp/src/armv4t"
  -I"$root/third_party/gbarecomp/src/runtime"
  -I"$src_dir"
)

mapfile -t shards < <(ls "$src_dir"/recompiled_*.cpp 2>/dev/null | sort)
if [[ ${#shards[@]} -lt 1 ]] || [[ ! -f "$src_dir/dispatch_table.cpp" ]]; then
  echo "error: expected dispatch_table.cpp and recompiled_*.cpp in $src_dir" >&2
  exit 1
fi

echo "==> compiling ${#shards[@]} shards + dispatch into $lib (jobs=$jobs)"
obj_dir="$artifact_dir/obj"
mkdir -p "$obj_dir"
objs=()

compile_one() {
  local src="$1"
  local base
  base="$(basename "$src" .cpp)"
  local obj="$obj_dir/${base}.o"
  if [[ -f "$obj" && "$obj" -nt "$src" ]]; then
    echo "  cached $base"
    return 0
  fi
  echo "  cc $base"
  c++ -std=c++20 -O2 -fPIC -shared -w \
    -fno-var-tracking -fno-var-tracking-assignments \
    "${gbarecomp_inc[@]}" \
    -c "$src" -o "$obj"
}

export -f compile_one
export obj_dir
export -p | grep -E '^gbarecomp_inc' >/dev/null 2>&1 || true

# Parallel compile without relying on GNU parallel.
pids=()
running=0
for src in "${shards[@]}" "$src_dir/dispatch_table.cpp"; do
  if [[ -f "$src_dir/symbol_map.cpp" && "$src" == "$src_dir/dispatch_table.cpp" ]]; then
    :
  fi
  (
    base="$(basename "$src" .cpp)"
    obj="$obj_dir/${base}.o"
    if [[ -f "$obj" && "$obj" -nt "$src" ]]; then
      exit 0
    fi
    echo "  cc $base"
    c++ -std=c++20 -O2 -fPIC -w \
      -fno-var-tracking -fno-var-tracking-assignments \
      -I"$root/third_party/gbarecomp/src/armv4t" \
      -I"$root/third_party/gbarecomp/src/runtime" \
      -I"$src_dir" \
      -c "$src" -o "$obj"
  ) &
  pids+=($!)
  running=$((running + 1))
  if [[ "$running" -ge "$jobs" ]]; then
    wait "${pids[0]}"
    pids=("${pids[@]:1}")
    running=$((running - 1))
  fi
done
if [[ -f "$src_dir/symbol_map.cpp" ]]; then
  (
    obj="$obj_dir/symbol_map.o"
    if [[ ! -f "$obj" || "$src_dir/symbol_map.cpp" -nt "$obj" ]]; then
      echo "  cc symbol_map"
      c++ -std=c++20 -O2 -fPIC -w \
        -fno-var-tracking -fno-var-tracking-assignments \
        -I"$root/third_party/gbarecomp/src/armv4t" \
        -I"$root/third_party/gbarecomp/src/runtime" \
        -I"$src_dir" \
        -c "$src_dir/symbol_map.cpp" -o "$obj"
    fi
  ) &
  pids+=($!)
fi
for pid in "${pids[@]+"${pids[@]}"}"; do
  wait "$pid"
done

mapfile -t objs < <(ls "$obj_dir"/*.o | sort)
echo "==> linking ${#objs[@]} objects -> $lib"
c++ -std=c++20 -O2 -shared -o "$lib" "${objs[@]}"
echo "$sha1" >"$artifact_dir/rom.sha1"
echo "Cart artifact ready: $lib"
echo "Link the host (fast if cart objects are unchanged):"
echo "  cmake -DGEN3RECOMP_CART_ARTIFACT=$lib -S \"$root\" -B \"$root/build\""
echo "  cmake --build \"$root/build\" -j$jobs --target gen3recomp"
echo "  ./build/gen3recomp --rom \"$rom\" --bios ./gba_bios.bin"
