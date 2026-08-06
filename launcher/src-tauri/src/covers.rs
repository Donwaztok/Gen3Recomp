use crate::catalog;
use crate::paths;
use base64::{engine::general_purpose::STANDARD as B64, Engine};
use std::fs;
use std::io::Read;
use std::path::Path;

fn looks_like_image(bytes: &[u8]) -> bool {
    bytes.starts_with(&[0x89, b'P', b'N', b'G'])
        || bytes.starts_with(&[0xFF, 0xD8, 0xFF])
        || (bytes.len() > 12 && &bytes[0..4] == b"RIFF" && &bytes[8..12] == b"WEBP")
}

pub fn find_cached_cover(game_id: &str, cwd: &Path) -> Option<std::path::PathBuf> {
    let local = paths::cover_local_override(game_id, cwd);
    if local.is_file() {
        return Some(local);
    }
    let jpg = cwd.join("roms").join("covers").join(format!("{game_id}.jpg"));
    if jpg.is_file() {
        return Some(jpg);
    }
    let cached = paths::cover_cache_path(game_id);
    if cached.is_file() {
        if let Ok(bytes) = fs::read(&cached) {
            if looks_like_image(&bytes) {
                return Some(cached);
            }
            // Stale HTML/error body from a previous 404/403 — drop it.
            let _ = fs::remove_file(&cached);
        }
    }
    None
}

fn download(url: &str, dest: &Path) -> Result<(), String> {
    if let Some(parent) = dest.parent() {
        fs::create_dir_all(parent).map_err(|e| e.to_string())?;
    }
    let tmp = dest.with_extension("partial");
    let client = reqwest::blocking::Client::builder()
        .timeout(std::time::Duration::from_secs(30))
        .user_agent(
            "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) \
             Chrome/122.0.0.0 Safari/537.36 gen3recomp-launcher/0.1",
        )
        .build()
        .map_err(|e| e.to_string())?;
    let mut resp = client
        .get(url)
        .header("Accept", "image/avif,image/webp,image/apng,image/*,*/*;q=0.8")
        .send()
        .map_err(|e| e.to_string())?;
    if !resp.status().is_success() {
        return Err(format!("HTTP {} for {url}", resp.status()));
    }
    let mut buf = Vec::new();
    resp.read_to_end(&mut buf).map_err(|e| e.to_string())?;
    if buf.len() < 1024 || !looks_like_image(&buf) {
        return Err(format!("download not a usable image: {url}"));
    }
    fs::write(&tmp, &buf).map_err(|e| e.to_string())?;
    fs::rename(&tmp, dest).map_err(|e| e.to_string())?;
    Ok(())
}

pub fn ensure_cover(game_id: &str, cwd: &Path) -> Result<std::path::PathBuf, String> {
    if let Some(existing) = find_cached_cover(game_id, cwd) {
        return Ok(existing);
    }
    let Some((primary, fallback)) = catalog::cover_urls(game_id) else {
        return Err("no curated cover URL".into());
    };
    let dest = paths::cover_cache_path(game_id);
    match download(&primary, &dest) {
        Ok(()) => Ok(dest),
        Err(e1) => match download(&fallback, &dest) {
            Ok(()) => Ok(dest),
            Err(e2) => Err(format!("{e1}; fallback: {e2}")),
        },
    }
}

/// Returns a data URL suitable for <img src>, or None for placeholder.
pub fn cover_data_url(game_id: &str, cwd: &Path, fetch: bool) -> Option<String> {
    let path = if fetch {
        ensure_cover(game_id, cwd).ok()
    } else {
        find_cached_cover(game_id, cwd)
    }?;
    let bytes = fs::read(&path).ok()?;
    if !looks_like_image(&bytes) {
        return None;
    }
    let mime = if bytes.starts_with(&[0x89, b'P', b'N', b'G']) {
        "image/png"
    } else if bytes.starts_with(&[0xFF, 0xD8, 0xFF]) {
        "image/jpeg"
    } else {
        "image/webp"
    };
    Some(format!("data:{mime};base64,{}", B64.encode(bytes)))
}
