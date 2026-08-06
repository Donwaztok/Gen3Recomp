#!/usr/bin/env bash
# Build host + launcher and assemble a GitHub Release zip (D7-safe: no ROM/BIOS/covers/cart).
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

version="${GEN3RECOMP_VERSION:-$(git describe --tags --always 2>/dev/null || echo 0.1.0)}"
stage="$root/dist/release/gen3recomp-${version}-linux-x64"
rm -rf "$stage"
mkdir -p "$stage/bin" "$stage/scripts" "$stage/docs"

echo "==> CMake host"
cmake -S . -B build
cmake --build build --target gen3recomp gba_recompile -j"$(nproc)"
install -m 755 build/gen3recomp "$stage/bin/gen3recomp"
if [[ -x build/_gbarecomp/gba_recompile ]]; then
  install -m 755 build/_gbarecomp/gba_recompile "$stage/bin/gba_recompile"
elif [[ -x build/gba_recompile ]]; then
  install -m 755 build/gba_recompile "$stage/bin/gba_recompile"
fi

echo "==> Tauri launcher (production embed — do not use bare cargo build)"
(
  cd launcher
  unset CARGO_TARGET_DIR
  export CARGO_TARGET_DIR="$root/launcher/src-tauri/target"
  npm ci 2>/dev/null || npm install
  npm run tauri build
)
launcher_bin=""
for candidate in \
  launcher/src-tauri/target/release/gen3recomp-launcher \
  launcher/src-tauri/target/release/bundle/deb/*/data/usr/bin/gen3recomp-launcher
do
  if [[ -x "$candidate" ]]; then
    launcher_bin="$candidate"
    break
  fi
done
if [[ -z "$launcher_bin" ]]; then
  echo "error: gen3recomp-launcher production binary missing after tauri build" >&2
  exit 1
fi
install -m 755 "$launcher_bin" "$stage/bin/gen3recomp-launcher"
install -m 755 scripts/run_launcher.sh "$stage/scripts/run_launcher.sh"
install -m 755 scripts/build_cart_artifact.sh "$stage/scripts/build_cart_artifact.sh" 2>/dev/null || true
# Wrapper next to binaries so double-click / PATH story is one command
cat > "$stage/gen3recomp-player" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
export PATH="$here/bin:$PATH"
export GEN3RECOMP_HOST="${GEN3RECOMP_HOST:-$here/bin/gen3recomp}"
export GEN3RECOMP_LAUNCHER="${GEN3RECOMP_LAUNCHER:-$here/bin/gen3recomp-launcher}"
export GDK_BACKEND="${GDK_BACKEND:-x11}"
export WEBKIT_DISABLE_COMPOSITING_MODE="${WEBKIT_DISABLE_COMPOSITING_MODE:-1}"
exec "$GEN3RECOMP_LAUNCHER" "$@"
EOF
chmod 755 "$stage/gen3recomp-player"

cp LICENSE "$stage/" 2>/dev/null || true
cp README.md "$stage/docs/" 2>/dev/null || true
cp docs/manual-boot.md "$stage/docs/" 2>/dev/null || true

mkdir -p "$root/dist/release"
zip_path="$root/dist/release/gen3recomp-${version}-linux-x64.zip"
rm -f "$zip_path"
(
  cd "$root/dist/release"
  zip -r "$(basename "$zip_path")" "$(basename "$stage")"
)

echo "==> Release folder: $stage"
echo "==> Zip: $zip_path"
echo "D7 check: no .gba / bios / covers / cart_aot in the zip (user supplies those)."
