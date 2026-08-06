#!/usr/bin/env bash
# Build host + launcher (optional) and assemble a D7-safe player package.
# Platforms: linux-x64 (default), macos-arm64, macos-x64
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

version="${GEN3RECOMP_VERSION:-$(git describe --tags --always 2>/dev/null || echo 0.1.0)}"
platform="${GEN3RECOMP_PLATFORM:-linux-x64}"
skip_build="${SKIP_BUILD:-0}"
jobs="${CMAKE_BUILD_PARALLEL_LEVEL:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

stage="$root/dist/release/gen3recomp-${version}-${platform}"
rm -rf "$stage"
mkdir -p "$stage/bin" "$stage/scripts" "$stage/docs" "$stage/roms"

host_src=""
launcher_src=""
gba_recompile_src=""

if [[ "$skip_build" != "1" ]]; then
  echo "==> CMake host"
  cmake -S . -B build
  cmake --build build --target gen3recomp gba_recompile -j"$jobs"

  echo "==> Tauri launcher (production embed — do not use bare cargo build)"
  (
    cd launcher
    unset CARGO_TARGET_DIR
    export CARGO_TARGET_DIR="$root/launcher/src-tauri/target"
    npm ci 2>/dev/null || npm install
    npm run tauri build
  )
fi

# Resolve binaries (local build tree or CI-provided paths)
if [[ -n "${GEN3RECOMP_HOST_BIN:-}" && -f "${GEN3RECOMP_HOST_BIN}" ]]; then
  host_src="$GEN3RECOMP_HOST_BIN"
elif [[ -x build/gen3recomp ]]; then
  host_src=build/gen3recomp
elif [[ -f build/Release/gen3recomp.exe ]]; then
  host_src=build/Release/gen3recomp.exe
elif [[ -f build/gen3recomp.exe ]]; then
  host_src=build/gen3recomp.exe
fi

if [[ -n "${GEN3RECOMP_LAUNCHER_BIN:-}" && -f "${GEN3RECOMP_LAUNCHER_BIN}" ]]; then
  launcher_src="$GEN3RECOMP_LAUNCHER_BIN"
else
  for candidate in \
    launcher/src-tauri/target/release/gen3recomp-launcher \
    launcher/src-tauri/target/release/gen3recomp-launcher.exe \
    launcher/src-tauri/target/release/bundle/macos/Gen3Recomp.app/Contents/MacOS/gen3recomp-launcher \
    launcher/src-tauri/target/release/bundle/deb/*/data/usr/bin/gen3recomp-launcher
  do
    if [[ -f "$candidate" ]]; then
      launcher_src="$candidate"
      break
    fi
  done
fi

if [[ -x build/_gbarecomp/gba_recompile ]]; then
  gba_recompile_src=build/_gbarecomp/gba_recompile
elif [[ -x build/gba_recompile ]]; then
  gba_recompile_src=build/gba_recompile
elif [[ -f build/Release/gba_recompile.exe ]]; then
  gba_recompile_src=build/Release/gba_recompile.exe
fi

if [[ -z "$host_src" ]]; then
  echo "error: gen3recomp host binary not found (build first or set GEN3RECOMP_HOST_BIN)" >&2
  exit 1
fi
if [[ -z "$launcher_src" ]]; then
  echo "error: gen3recomp-launcher production binary missing after tauri build" >&2
  exit 1
fi

host_name="$(basename "$host_src")"
launcher_name="$(basename "$launcher_src")"
install -m 755 "$host_src" "$stage/bin/$host_name"
install -m 755 "$launcher_src" "$stage/bin/$launcher_name"
if [[ -n "$gba_recompile_src" ]]; then
  install -m 755 "$gba_recompile_src" "$stage/bin/$(basename "$gba_recompile_src")"
fi

install -m 755 scripts/run_launcher.sh "$stage/scripts/run_launcher.sh" 2>/dev/null || true
install -m 755 scripts/build_cart_artifact.sh "$stage/scripts/build_cart_artifact.sh" 2>/dev/null || true
install -m 755 scripts/verify_release_layout.sh "$stage/scripts/verify_release_layout.sh" 2>/dev/null || true

cat > "$stage/roms/README.txt" <<'EOF'
Place catalogued USA Ruby, Sapphire, or Emerald .gba dumps in this folder.
Place gba_bios.bin in the package root (next to gen3recomp-player).

This project does not distribute Nintendo ROMs, BIOS images, or cover art.
EOF

# Player entrypoint
cat > "$stage/gen3recomp-player" <<EOF
#!/usr/bin/env bash
set -euo pipefail
here="\$(cd "\$(dirname "\$0")" && pwd)"
export PATH="\$here/bin:\$PATH"
export GEN3RECOMP_HOST="\${GEN3RECOMP_HOST:-\$here/bin/$host_name}"
export GEN3RECOMP_LAUNCHER="\${GEN3RECOMP_LAUNCHER:-\$here/bin/$launcher_name}"
export GDK_BACKEND="\${GDK_BACKEND:-x11}"
export WEBKIT_DISABLE_COMPOSITING_MODE="\${WEBKIT_DISABLE_COMPOSITING_MODE:-1}"
# Prefer package-root CWD so roms/ and gba_bios.bin resolve naturally
cd "\$here"
exec "\$GEN3RECOMP_LAUNCHER" "\$@"
EOF
chmod 755 "$stage/gen3recomp-player"

cp LICENSE "$stage/" 2>/dev/null || true
cp README.md "$stage/docs/" 2>/dev/null || true
cp docs/manual-boot.md "$stage/docs/" 2>/dev/null || true
if [[ -f docs/player-guide.md ]]; then
  cp docs/player-guide.md "$stage/docs/"
fi

mkdir -p "$root/dist/release"
archive_base="gen3recomp-${version}-${platform}"
zip_path="$root/dist/release/${archive_base}.zip"
rm -f "$zip_path"
(
  cd "$root/dist/release"
  if command -v zip >/dev/null 2>&1; then
    zip -r "$(basename "$zip_path")" "$(basename "$stage")"
  else
    tar -czf "${archive_base}.tar.gz" "$(basename "$stage")"
    zip_path="$root/dist/release/${archive_base}.tar.gz"
  fi
)

bash "$root/scripts/verify_release_layout.sh" "$stage"

echo "==> Release folder: $stage"
echo "==> Archive: $zip_path"
echo "D7 check: no .gba / bios / covers / cart_aot in the package (user supplies those)."
