use crate::paths;
use serde::Serialize;
use std::collections::HashSet;
use std::fs;
use std::io::{BufRead, Write};
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

#[derive(Clone, Debug, Serialize)]
pub struct ModInfo {
    pub id: String,
    pub name: String,
    pub version: String,
    pub enabled: bool,
    pub valid: bool,
    pub invalid_reason: String,
}

fn mods_roots(cwd: &Path, repo_root: Option<&Path>) -> Vec<PathBuf> {
    let mut roots = vec![
        paths::user_data_dir().join("mods"),
        cwd.join("mods"),
    ];
    if let Some(root) = repo_root {
        let p = root.join("mods");
        if !roots.iter().any(|r| r == &p) {
            roots.push(p);
        }
    }
    roots
}

fn enabled_set() -> HashSet<String> {
    let path = paths::user_data_dir().join("mods_enabled.txt");
    let Ok(file) = fs::File::open(path) else {
        return HashSet::new();
    };
    let mut set = HashSet::new();
    for line in std::io::BufReader::new(file).lines().flatten() {
        let t = line.trim();
        if !t.is_empty() && !t.starts_with('#') {
            set.insert(t.to_string());
        }
    }
    set
}

fn write_enabled_set(ids: &HashSet<String>) -> Result<(), String> {
    let dir = paths::user_data_dir();
    fs::create_dir_all(&dir).map_err(|e| e.to_string())?;
    let path = dir.join("mods_enabled.txt");
    let mut out = fs::File::create(path).map_err(|e| e.to_string())?;
    let mut sorted: Vec<_> = ids.iter().collect();
    sorted.sort();
    for id in sorted {
        writeln!(out, "{id}").map_err(|e| e.to_string())?;
    }
    Ok(())
}

fn package_has_forbidden(root: &Path) -> Option<String> {
    for entry in WalkDir::new(root).into_iter().flatten() {
        if !entry.file_type().is_file() {
            continue;
        }
        let name = entry.file_name().to_string_lossy().to_ascii_lowercase();
        if name.ends_with(".gba") || name.contains("bios") && name.ends_with(".bin") {
            return Some("mod packages must not embed ROM or BIOS payloads".into());
        }
    }
    None
}

fn parse_simple_toml_name_version(path: &Path) -> (String, String) {
    let Ok(text) = fs::read_to_string(path) else {
        return (String::new(), String::new());
    };
    let mut name = String::new();
    let mut version = String::new();
    for line in text.lines() {
        let line = line.trim();
        if line.is_empty() || line.starts_with('#') || line.starts_with('[') {
            continue;
        }
        let Some((k, v)) = line.split_once('=') else {
            continue;
        };
        let key = k.trim();
        let mut val = v.trim().to_string();
        if val.starts_with('"') && val.ends_with('"') && val.len() >= 2 {
            val = val[1..val.len() - 1].to_string();
        }
        if key == "name" {
            name = val;
        } else if key == "version" {
            version = val;
        }
    }
    (name, version)
}

pub fn discover_mods(cwd: &Path, repo_root: Option<&Path>) -> Vec<ModInfo> {
    let enabled = enabled_set();
    let mut out = Vec::new();
    let mut seen = HashSet::new();
    for root in mods_roots(cwd, repo_root) {
        let Ok(entries) = fs::read_dir(&root) else {
            continue;
        };
        for entry in entries.flatten() {
            let path = entry.path();
            if !path.is_dir() {
                continue;
            }
            let id = entry.file_name().to_string_lossy().to_string();
            if !seen.insert(id.clone()) {
                continue;
            }
            let manifest = path.join("mod.toml");
            if !manifest.is_file() {
                continue;
            }
            let (mut name, mut version) = parse_simple_toml_name_version(&manifest);
            if name.is_empty() {
                name = id.clone();
            }
            if version.is_empty() {
                version = "0".into();
            }
            let mut valid = true;
            let mut invalid_reason = String::new();
            if let Some(reason) = package_has_forbidden(&path) {
                valid = false;
                invalid_reason = reason;
            }
            out.push(ModInfo {
                id: id.clone(),
                name,
                version,
                enabled: enabled.contains(&id) && valid,
                valid,
                invalid_reason,
            });
        }
    }
    out.sort_by(|a, b| a.id.cmp(&b.id));
    out
}

pub fn set_mod_enabled(id: &str, enabled: bool, cwd: &Path, repo_root: Option<&Path>) -> Result<(), String> {
    let mods = discover_mods(cwd, repo_root);
    let Some(mod_info) = mods.iter().find(|m| m.id == id) else {
        return Err(format!("unknown mod: {id}"));
    };
    if enabled && !mod_info.valid {
        return Err(if mod_info.invalid_reason.is_empty() {
            "mod is not valid".into()
        } else {
            mod_info.invalid_reason.clone()
        });
    }
    let mut set = enabled_set();
    if enabled {
        set.insert(id.to_string());
    } else {
        set.remove(id);
    }
    write_enabled_set(&set)
}
