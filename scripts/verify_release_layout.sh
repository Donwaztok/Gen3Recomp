#!/usr/bin/env bash
# Verify a staged player package (or extracted archive) is D7-safe and complete.
set -euo pipefail

stage="${1:-}"
if [[ -z "$stage" || ! -d "$stage" ]]; then
  echo "usage: $0 <stage-dir>" >&2
  exit 2
fi

fail=0
host=""
launcher=""
for name in gen3recomp gen3recomp.exe; do
  if [[ -f "$stage/bin/$name" ]]; then
    host="$stage/bin/$name"
    break
  fi
done
for name in gen3recomp-launcher gen3recomp-launcher.exe; do
  if [[ -f "$stage/bin/$name" ]]; then
    launcher="$stage/bin/$name"
    break
  fi
done

if [[ -z "$host" ]]; then
  echo "error: missing host binary under bin/" >&2
  fail=1
fi
if [[ -z "$launcher" ]]; then
  echo "error: missing launcher binary under bin/" >&2
  fail=1
fi
if [[ ! -d "$stage/roms" ]]; then
  echo "error: missing roms/ directory" >&2
  fail=1
fi
if [[ ! -f "$stage/roms/README.txt" ]]; then
  echo "error: missing roms/README.txt placeholder" >&2
  fail=1
fi

# D7: no illegal payloads
while IFS= read -r -d '' f; do
  echo "error: illegal payload in package: $f" >&2
  fail=1
done < <(find "$stage" \( \
  -iname '*.gba' -o -iname '*.agb' -o \
  -iname 'gba_bios.bin' -o -iname '*bios*.bin' -o \
  -iname 'libcart.so' -o -iname 'cart.dll' -o -iname 'libcart.dylib' -o \
  -path '*/cart_aot/*' -o -path '*/covers/*' \
\) -print0 2>/dev/null || true)

if [[ "$fail" -ne 0 ]]; then
  exit 1
fi

echo "OK: layout check passed for $stage"
