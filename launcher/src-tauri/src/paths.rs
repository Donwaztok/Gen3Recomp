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

pub fn find_host_binary(repo_root: Option<&Path>) -> Option<PathBuf> {
    if let Ok(override_path) = std::env::var("GEN3RECOMP_HOST") {
        let p = PathBuf::from(override_path);
        if p.is_file() {
            return Some(p);
        }
    }
    let mut candidates: Vec<PathBuf> = Vec::new();
    if let Ok(cwd) = std::env::current_dir() {
        candidates.push(cwd.join("build/gen3recomp"));
        candidates.push(cwd.join("gen3recomp"));
    }
    if let Some(root) = repo_root {
        candidates.push(root.join("build/gen3recomp"));
    }
    if let Ok(exe) = std::env::current_exe() {
        if let Some(dir) = exe.parent() {
            candidates.push(dir.join("gen3recomp"));
        }
    }
    candidates.into_iter().find(|p| p.is_file())
}

pub fn resolve_bios_path(cwd: &Path) -> Option<PathBuf> {
    let local = cwd.join("gba_bios.bin");
    if local.is_file() {
        return Some(local);
    }
    None
}

pub fn roms_dirs(cwd: &Path, repo_root: Option<&Path>) -> Vec<PathBuf> {
    let mut dirs = vec![cwd.join("roms")];
    if let Some(root) = repo_root {
        let p = root.join("roms");
        if !dirs.iter().any(|d| d == &p) {
            dirs.push(p);
        }
    }
    dirs
}
