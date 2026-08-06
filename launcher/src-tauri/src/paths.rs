use std::path::{Path, PathBuf};

pub fn user_data_dir() -> PathBuf {
    if let Ok(xdg) = std::env::var("XDG_DATA_HOME") {
        if !xdg.is_empty() {
            return PathBuf::from(xdg).join("gen3recomp");
        }
    }
    #[cfg(target_os = "windows")]
    {
        if let Ok(appdata) = std::env::var("APPDATA") {
            if !appdata.is_empty() {
                return PathBuf::from(appdata).join("gen3recomp");
            }
        }
    }
    #[cfg(target_os = "macos")]
    {
        if let Some(home) = dirs::home_dir() {
            return home.join("Library/Application Support/gen3recomp");
        }
    }
    #[cfg(not(any(target_os = "windows", target_os = "macos")))]
    {
        if let Some(home) = dirs::home_dir() {
            return home.join(".local/share/gen3recomp");
        }
    }
    std::env::temp_dir().join("gen3recomp")
}

pub fn cart_artifact_abi_tag() -> &'static str {
    #[cfg(target_os = "windows")]
    {
        "abi3-windows-x64"
    }
    #[cfg(all(target_os = "macos", target_arch = "aarch64"))]
    {
        "abi3-macos-arm64"
    }
    #[cfg(all(target_os = "macos", not(target_arch = "aarch64")))]
    {
        "abi3-macos-x64"
    }
    #[cfg(not(any(target_os = "windows", target_os = "macos")))]
    {
        "abi3-linux-x64"
    }
}

pub fn cart_artifact_library_name() -> &'static str {
    #[cfg(target_os = "windows")]
    {
        "cart.dll"
    }
    #[cfg(target_os = "macos")]
    {
        "libcart.dylib"
    }
    #[cfg(not(any(target_os = "windows", target_os = "macos")))]
    {
        "libcart.so"
    }
}

pub fn cart_artifact_path(rom_sha1: &str) -> PathBuf {
    user_data_dir()
        .join("cart_aot")
        .join(rom_sha1)
        .join(cart_artifact_abi_tag())
        .join(cart_artifact_library_name())
}

pub fn cover_cache_path(game_id: &str) -> PathBuf {
    user_data_dir().join("covers").join(format!("{game_id}.img"))
}

pub fn cover_local_override(game_id: &str, cwd: &Path) -> PathBuf {
    cwd.join("roms").join("covers").join(format!("{game_id}.png"))
}

/// Walk upward from `start` looking for the repo root (has scripts/build_cart_artifact.sh).
pub fn find_repo_root(start: &Path) -> Option<PathBuf> {
    let mut cur = start.to_path_buf();
    for _ in 0..8 {
        if cur.join("scripts/build_cart_artifact.sh").is_file() {
            return Some(cur);
        }
        if !cur.pop() {
            break;
        }
    }
    None
}

/// Package/install root: parent of `bin/` when the binary lives under `bin/`, else the
/// directory containing the executable (Release layout or side-by-side binaries).
pub fn install_root_from_binary(binary: &Path) -> Option<PathBuf> {
    let dir = binary.parent()?;
    if dir.file_name().and_then(|n| n.to_str()) == Some("bin") {
        dir.parent().map(|p| p.to_path_buf())
    } else {
        Some(dir.to_path_buf())
    }
}

fn looks_like_package_root(root: &Path) -> bool {
    root.join("bin").is_dir()
        || root.join("gen3recomp-player").is_file()
        || root.join("gen3recomp-player.bat").is_file()
        || root.join("gen3recomp-player.sh").is_file()
}

fn host_binary_names() -> &'static [&'static str] {
    #[cfg(target_os = "windows")]
    {
        &["gen3recomp.exe", "gen3recomp"]
    }
    #[cfg(not(target_os = "windows"))]
    {
        &["gen3recomp"]
    }
}

pub fn find_host_binary(repo_root: Option<&Path>) -> Option<PathBuf> {
    if let Ok(override_path) = std::env::var("GEN3RECOMP_HOST") {
        let p = PathBuf::from(override_path);
        if p.is_file() {
            return Some(p);
        }
    }
    let names = host_binary_names();
    let mut candidates: Vec<PathBuf> = Vec::new();
    if let Ok(cwd) = std::env::current_dir() {
        for name in names {
            candidates.push(cwd.join("build").join(name));
            candidates.push(cwd.join("bin").join(name));
            candidates.push(cwd.join(name));
        }
    }
    if let Some(root) = repo_root {
        for name in names {
            candidates.push(root.join("build").join(name));
            candidates.push(root.join("bin").join(name));
        }
    }
    if let Ok(exe) = std::env::current_exe() {
        if let Some(dir) = exe.parent() {
            for name in names {
                candidates.push(dir.join(name));
            }
            // Package layout: launcher in bin/, host beside it
            if dir.file_name().and_then(|n| n.to_str()) == Some("bin") {
                for name in names {
                    candidates.push(dir.join(name));
                }
            }
        }
    }
    candidates.into_iter().find(|p| p.is_file())
}

fn push_unique(dirs: &mut Vec<PathBuf>, path: PathBuf) {
    if !dirs.iter().any(|d| d == &path) {
        dirs.push(path);
    }
}

/// ROM scan roots, in priority order (D4):
/// 1. Package/install root `roms/`
/// 2. Host-binary parent `roms/` (covers `build/roms` and `bin/roms`)
/// 3. CWD / repo-root `roms/`
pub fn roms_dirs(cwd: &Path, repo_root: Option<&Path>, host: Option<&Path>) -> Vec<PathBuf> {
    let mut dirs = Vec::new();

    if let Ok(exe) = std::env::current_exe() {
        if let Some(root) = install_root_from_binary(&exe) {
            push_unique(&mut dirs, root.join("roms"));
        }
        if let Some(dir) = exe.parent() {
            push_unique(&mut dirs, dir.join("roms"));
        }
    }

    if let Some(host) = host {
        if let Some(root) = install_root_from_binary(host) {
            push_unique(&mut dirs, root.join("roms"));
        }
        if let Some(dir) = host.parent() {
            push_unique(&mut dirs, dir.join("roms"));
        }
    }

    push_unique(&mut dirs, cwd.join("roms"));
    if let Some(root) = repo_root {
        push_unique(&mut dirs, root.join("roms"));
    }

    dirs
}

/// Prefer package/build `roms/` for Add ROM copies; fall back to CWD.
pub fn preferred_roms_dir(cwd: &Path, repo_root: Option<&Path>, host: Option<&Path>) -> PathBuf {
    if let Ok(exe) = std::env::current_exe() {
        if let Some(root) = install_root_from_binary(&exe) {
            if looks_like_package_root(&root) {
                return root.join("roms");
            }
        }
    }
    if let Some(host) = host {
        if let Some(dir) = host.parent() {
            let name = dir.file_name().and_then(|n| n.to_str()).unwrap_or("");
            if name == "build" {
                return dir.join("roms");
            }
            if name == "bin" {
                if let Some(pkg) = dir.parent() {
                    return pkg.join("roms");
                }
            }
        }
        if let Some(root) = install_root_from_binary(host) {
            if looks_like_package_root(&root) {
                return root.join("roms");
            }
        }
    }
    if let Some(root) = repo_root {
        let p = root.join("roms");
        if p.is_dir() {
            return p;
        }
    }
    cwd.join("roms")
}

/// Resolve BIOS from package root, host parent, then CWD.
pub fn resolve_bios_path(cwd: &Path, host: Option<&Path>) -> Option<PathBuf> {
    let mut candidates: Vec<PathBuf> = Vec::new();

    if let Ok(exe) = std::env::current_exe() {
        if let Some(root) = install_root_from_binary(&exe) {
            candidates.push(root.join("gba_bios.bin"));
        }
        if let Some(dir) = exe.parent() {
            candidates.push(dir.join("gba_bios.bin"));
        }
    }

    if let Some(host) = host {
        if let Some(root) = install_root_from_binary(host) {
            candidates.push(root.join("gba_bios.bin"));
        }
        if let Some(dir) = host.parent() {
            candidates.push(dir.join("gba_bios.bin"));
        }
    }

    candidates.push(cwd.join("gba_bios.bin"));
    candidates.into_iter().find(|p| p.is_file())
}
