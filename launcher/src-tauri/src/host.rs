use crate::catalog;
use crate::covers;
use crate::mods;
use crate::paths;
use serde::Serialize;
use sha1::{Digest, Sha1};
use std::fs;
use std::io::{BufRead, BufReader};
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};
use std::sync::atomic::{AtomicBool, Ordering};
use tauri::{AppHandle, Emitter};

/// Single-flight guard: only one cart AOT Build may run at a time.
static BUILD_IN_FLIGHT: AtomicBool = AtomicBool::new(false);

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
    let host = paths::find_host_binary(repo.as_deref());
    let mut roms = Vec::new();
    let mut seen = std::collections::HashSet::new();

    for dir in paths::roms_dirs(&cwd, repo.as_deref(), host.as_deref()) {
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

    let bios = match paths::resolve_bios_path(&cwd, host.as_deref()) {
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
            message: "Place gba_bios.bin next to the player package (or in the working directory)"
                .into(),
        },
    };

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
    let (cwd, repo) = context();
    let host = paths::find_host_binary(repo.as_deref());
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
    // Copy into preferred package/build roms/ when possible
    let dest_dir = paths::preferred_roms_dir(&cwd, repo.as_deref(), host.as_deref());
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

#[derive(Clone, Debug, Serialize)]
pub struct BuildProgressEvent {
    pub sha1: String,
    pub phase: String,
    pub current: u32,
    pub total: u32,
    pub percent: f32,
    pub message: String,
}

fn phase_percent(phase: &str, current: u32, total: u32) -> f32 {
    let ratio = if total == 0 {
        0.0
    } else {
        (current as f32 / total as f32).clamp(0.0, 1.0)
    };
    match phase {
        "generate" => 10.0 * ratio,
        "compile" => 10.0 + 80.0 * ratio,
        "link" => 90.0 + 10.0 * ratio,
        "done" => 100.0,
        _ => (100.0 * ratio).clamp(0.0, 99.0),
    }
}

fn parse_progress_line(line: &str, sha1: &str) -> Option<BuildProgressEvent> {
    let rest = line.strip_prefix("PROGRESS ")?;
    let mut current = 0u32;
    let mut total = 0u32;
    let mut phase = String::from("unknown");
    for part in rest.split_whitespace() {
        if let Some(v) = part.strip_prefix("current=") {
            current = v.parse().unwrap_or(0);
        } else if let Some(v) = part.strip_prefix("total=") {
            total = v.parse().unwrap_or(0);
        } else if let Some(v) = part.strip_prefix("phase=") {
            phase = v.to_string();
        }
    }
    let percent = phase_percent(&phase, current, total);
    let message = match phase.as_str() {
        "generate" => "Generating cart sources…".into(),
        "compile" => format!("Compiling shards ({current}/{total})…"),
        "link" => "Linking libcart.so…".into(),
        "done" => "Cart artifact ready".into(),
        _ => format!("{phase} {current}/{total}"),
    };
    Some(BuildProgressEvent {
        sha1: sha1.to_string(),
        phase,
        current,
        total,
        percent,
        message,
    })
}

struct BuildGuard;

impl BuildGuard {
    fn try_acquire() -> Result<Self, String> {
        if BUILD_IN_FLIGHT
            .compare_exchange(false, true, Ordering::SeqCst, Ordering::SeqCst)
            .is_err()
        {
            return Err(
                "A cart AOT Build is already running. Wait for it to finish before starting another."
                    .into(),
            );
        }
        Ok(Self)
    }
}

impl Drop for BuildGuard {
    fn drop(&mut self) {
        BUILD_IN_FLIGHT.store(false, Ordering::SeqCst);
    }
}

pub fn build_cart(app: &AppHandle, rom_path: &str) -> Result<String, String> {
    let _guard = BuildGuard::try_acquire()?;

    let (cwd, repo) = context();
    let Some(root) = repo else {
        return Err("could not find repository root (scripts/build_cart_artifact.sh)".into());
    };
    let script = root.join("scripts/build_cart_artifact.sh");
    if !script.is_file() {
        return Err("build_cart_artifact.sh missing".into());
    }

    let rom = PathBuf::from(rom_path);
    if !rom.is_file() {
        return Err("ROM file not found".into());
    }
    let sha1 = sha1_file(&rom)?;
    if catalog::find_by_sha1(&sha1).is_none() {
        return Err(format!(
            "unsupported ROM dump (SHA-1 {sha1}). MVP supports USA Ruby, Sapphire, and Emerald only."
        ));
    }

    let _ = app.emit(
        "build-progress",
        BuildProgressEvent {
            sha1: sha1.clone(),
            phase: "generate".into(),
            current: 0,
            total: 1,
            percent: 0.0,
            message: "Starting cart AOT build…".into(),
        },
    );

    // Prefer line-buffered stdout so PROGRESS lines arrive while compiling.
    let mut child = Command::new("stdbuf")
        .args(["-oL", "-eL", "bash"])
        .arg(&script)
        .arg(rom_path)
        .current_dir(&root)
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn();

    if child.is_err() {
        child = Command::new("bash")
            .arg(&script)
            .arg(rom_path)
            .current_dir(&root)
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn();
    }

    let mut child = child.map_err(|e| e.to_string())?;
    let stdout = child.stdout.take().ok_or("missing stdout")?;
    let stderr = child.stderr.take().ok_or("missing stderr")?;

    let app_out = app.clone();
    let sha1_out = sha1.clone();
    let stdout_thread = std::thread::spawn(move || {
        let mut log = String::new();
        let reader = BufReader::new(stdout);
        for line in reader.lines().flatten() {
            log.push_str(&line);
            log.push('\n');
            if let Some(ev) = parse_progress_line(line.trim(), &sha1_out) {
                let _ = app_out.emit("build-progress", ev);
            }
        }
        log
    });

    let stderr_thread = std::thread::spawn(move || {
        let mut log = String::new();
        let reader = BufReader::new(stderr);
        for line in reader.lines().flatten() {
            log.push_str(&line);
            log.push('\n');
        }
        log
    });

    let status = child.wait().map_err(|e| e.to_string())?;
    let stdout_log = stdout_thread.join().unwrap_or_default();
    let stderr_log = stderr_thread.join().unwrap_or_default();

    if !status.success() {
        return Err(format!(
            "Build failed (exit {:?})\n{stderr_log}\n{stdout_log}",
            status.code()
        ));
    }

    let _ = app.emit(
        "build-progress",
        BuildProgressEvent {
            sha1,
            phase: "done".into(),
            current: 1,
            total: 1,
            percent: 100.0,
            message: "Cart artifact ready".into(),
        },
    );

    let _ = cwd;
    Ok(if stdout_log.is_empty() {
        stderr_log
    } else {
        stdout_log
    })
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
