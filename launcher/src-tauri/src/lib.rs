mod catalog;
mod covers;
mod host;
mod mods;
mod paths;

use host::{BiosStatus, LibraryState, RomEntry};

#[tauri::command]
fn refresh_library(fetch_covers: bool) -> LibraryState {
    host::refresh_library(fetch_covers)
}

#[tauri::command]
fn identify_rom(path: String) -> Result<RomEntry, String> {
    host::identify_rom_path(&path)
}

#[tauri::command]
fn build_cart(rom_path: String) -> Result<String, String> {
    host::build_cart(&rom_path)
}

#[tauri::command]
fn play_rom(rom_path: String, bios_path: String) -> Result<(), String> {
    host::play_rom(&rom_path, &bios_path)
}

#[tauri::command]
fn set_mod_enabled(id: String, enabled: bool) -> Result<(), String> {
    host::set_mod(id.as_str(), enabled)
}

#[tauri::command]
fn fetch_cover(game_id: String) -> Result<Option<String>, String> {
    host::fetch_cover(&game_id)
}

#[tauri::command]
fn bios_status() -> BiosStatus {
    host::refresh_library(false).bios
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_dialog::init())
        .invoke_handler(tauri::generate_handler![
            refresh_library,
            identify_rom,
            build_cart,
            play_rom,
            set_mod_enabled,
            fetch_cover,
            bios_status
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
