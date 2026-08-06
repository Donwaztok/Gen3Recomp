use crate::catalog;
use crate::covers;
use crate::mods;
use crate::paths;
use serde::Serialize;
use sha1::{Digest, Sha1};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

#[derive(Clone, Debug, Serialize)]
pub struct RomEntry {
    pub path: String,
    pub sha1: String,
    pub game_id: String,
    pub display_name: String,
    pub region: String,
    pub aot_ready: bool,
    pub cover_data_url: Option<String>,
}

#[derive(Clone, Debug, Serialize)]
pub struct BiosStatus {
    pub present: bool,
    pub path: Option<String>,
    pub valid: bool,
    pub message: String,
}

#[derive(Clone, Debug, Serialize)]
pub struct LibraryState {
    pub roms: Vec<RomEntry>,
    pub bios: BiosStatus,
    pub mods: Vec<mods::ModInfo>,
    pub repo_root: Option<String>,
    pub host_path: Option<String>,
    pub message: String,
}

fn sha1_file(path: &Path) -> Result<String, String> {
    let bytes = fs::read(path).map_err(|e| e.to_string())?;
    let mut hasher = Sha1::new();
    hasher.update(&bytes);
    Ok(hex::encode(hasher.finalize()))
}

fn context() -> (PathBuf, Option<PathBuf>) {
    let cwd = std::env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    let repo = paths::find_repo_root(&cwd).or_else(|| {
        std::env::current_exe()
            .ok()
            .and_then(|e| e.parent().map(|p| p.to_path_buf()))
            .and_then(|p| paths::find_repo_root(&p))
    });
    (cwd, repo)
}

pub fn refresh_library(fetch_covers: bool) -> LibraryState {
    let (cwd, repo) = context();
    let mut roms = Vec::new();
    let mut seen = std::collections::HashSet::new();

    for dir in paths::roms_dirs(&cwd, repo.as_deref()) {
        let Ok(entries) = fs::read_dir(&dir) else {
            continue;
        };
        for entry in entries.flatten() {
            let path = entry.path();
            if !path.is_file() {
                continue;
            }
            let ext = path
                .extension()
                .and_then(|e| e.to_str())
                .unwrap_or("")
                .to_ascii_lowercase();
            if ext != "gba" {
                continue;
            }
            let Ok(sha1) = sha1_file(&path) else {
                continue;
            };
            let Some(game) = catalog::find_by_sha1(&sha1) else {
                continue;
            };
            if !seen.insert(sha1.clone()) {
                continue;
            }
            let cover = covers::cover_data_url(&game.id, &cwd, fetch_covers);
            roms.push(RomEntry {
                path: path.display().to_string(),
                sha1: sha1.clone(),
                game_id: game.id.clone(),
                display_name: game.display_name,
                region: game.region,
                aot_ready: paths::cart_artifact_path(&sha1).is_file(),
                cover_data_url: cover,
            });
        }
    }
    roms.sort_by(|a, b| a.display_name.cmp(&b.display_name).then(a.sha1.cmp(&b.sha1)));

    let bios = match paths::resolve_bios_path(&cwd) {
        Some(p) => match sha1_file(&p) {
            Ok(sha) if catalog::is_known_bios_sha1(&sha) => BiosStatus {
                present: true,
                path: Some(p.display().to_string()),
                valid: true,
                message: "BIOS ready".into(),
            },
            Ok(_) => BiosStatus {
                present: true,
                path: Some(p.display().to_string()),
                valid: false,
                message: "BIOS present but SHA-1 is not a known dump".into(),
            },
            Err(e) => BiosStatus {
                present: true,
                path: Some(p.display().to_string()),
                valid: false,
                message: e,
            },
        },
        None => BiosStatus {
            present: false,
            path: None,
            valid: false,
            message: "Place gba_bios.bin in the working directory".into(),
        },
    };

    let host = paths::find_host_binary(repo.as_deref());
    LibraryState {
        mods: mods::discover_mods(&cwd, repo.as_deref()),
        roms,
        bios,
        repo_root: repo.as_ref().map(|p| p.display().to_string()),
        host_path: host.map(|p| p.display().to_string()),
        message: String::new(),
    }
}

pub fn identify_rom_path(path: &str) -> Result<RomEntry, String> {
    let (cwd, _) = context();
    let path = PathBuf::from(path);
    if !path.is_file() {
        return Err("file not found".into());
    }
    let sha1 = sha1_file(&path)?;
    let Some(game) = catalog::find_by_sha1(&sha1) else {
        return Err(format!(
            "unsupported ROM dump (SHA-1 {sha1}). MVP supports USA Ruby, Sapphire, and Emerald only."
        ));
    };
    // Copy into roms/ if not already there
    let dest_dir = cwd.join("roms");
    fs::create_dir_all(&dest_dir).map_err(|e| e.to_string())?;
    let dest = dest_dir.join(path.file_name().unwrap_or_default());
    if path != dest {
        fs::copy(&path, &dest).map_err(|e| e.to_string())?;
    }
    let cover = covers::cover_data_url(&game.id, &cwd, true);
    Ok(RomEntry {
        path: dest.display().to_string(),
        sha1: sha1.clone(),
        game_id: game.id,
        display_name: game.display_name,
        region: game.region,
        aot_ready: paths::cart_artifact_path(&sha1).is_file(),
        cover_data_url: cover,
    })
}

pub fn build_cart(rom_path: &str) -> Result<String, String> {
    let (cwd, repo) = context();
    let Some(root) = repo else {
        return Err("could not find repository root (scripts/build_cart_artifact.sh)".into());
    };
    let script = root.join("scripts/build_cart_artifact.sh");
    if !script.is_file() {
        return Err("build_cart_artifact.sh missing".into());
    }
    let output = Command::new("bash")
        .arg(&script)
        .arg(rom_path)
        .current_dir(&root)
        .output()
        .map_err(|e| e.to_string())?;
    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();
    if !output.status.success() {
        return Err(format!(
            "Build failed (exit {:?})\n{stderr}\n{stdout}",
            output.status.code()
        ));
    }
    let _ = cwd;
    Ok(if stdout.is_empty() { stderr } else { stdout })
}

pub fn play_rom(rom_path: &str, bios_path: &str) -> Result<(), String> {
    let (_, repo) = context();
    let Some(host) = paths::find_host_binary(repo.as_deref()) else {
        return Err(
            "gen3recomp host not found. Build it with cmake, or set GEN3RECOMP_HOST.".into(),
        );
    };
    Command::new(&host)
        .arg("--rom")
        .arg(rom_path)
        .arg("--bios")
        .arg(bios_path)
        .spawn()
        .map_err(|e| format!("failed to spawn {}: {e}", host.display()))?;
    Ok(())
}

pub fn set_mod(id: &str, enabled: bool) -> Result<(), String> {
    let (cwd, repo) = context();
    mods::set_mod_enabled(id, enabled, &cwd, repo.as_deref())
}

pub fn fetch_cover(game_id: &str) -> Result<Option<String>, String> {
    let (cwd, _) = context();
    Ok(covers::cover_data_url(game_id, &cwd, true))
}
