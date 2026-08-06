# gen3recomp-launcher

Tauri 2 + React + HeroUI player shell for gen3recomp.

```sh
npm install
npm run tauri:dev
npm run tauri:build   # → src-tauri/target/release/gen3recomp-launcher
```

Requires Node 20+, Rust (rustup), and a WebView (Linux: `webkit2gtk-4.1`).

The launcher scans `roms/`, fetches covers into user-data, runs `scripts/build_cart_artifact.sh`, and spawns `../build/gen3recomp --rom … --bios …`.
