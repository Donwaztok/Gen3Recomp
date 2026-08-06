use serde::Serialize;

#[derive(Clone, Debug, Serialize)]
pub struct CatalogEntry {
    pub id: String,
    pub display_name: String,
    pub region: String,
    pub sha1: String,
}

pub fn builtin() -> Vec<CatalogEntry> {
    vec![
        CatalogEntry {
            id: "ruby-usa".into(),
            display_name: "Pokémon Ruby".into(),
            region: "USA".into(),
            sha1: "f28b6ffc97847e94a6c21a63cacf633ee5c8df1e".into(),
        },
        CatalogEntry {
            id: "sapphire-usa".into(),
            display_name: "Pokémon Sapphire".into(),
            region: "USA".into(),
            sha1: "3ccbbd45f8553c36463f13b938e833f652b793e4".into(),
        },
        CatalogEntry {
            id: "sapphire-usa-rev1".into(),
            display_name: "Pokémon Sapphire".into(),
            region: "USA".into(),
            sha1: "4722efb8cd45772ca32555b98fd3b9719f8e60a9".into(),
        },
        CatalogEntry {
            id: "emerald-usa".into(),
            display_name: "Pokémon Emerald".into(),
            region: "USA".into(),
            sha1: "f3ae088181bf583e55daf962a92bb46f4f1d07b7".into(),
        },
    ]
}

pub fn find_by_sha1(sha1: &str) -> Option<CatalogEntry> {
    let needle = sha1.to_ascii_lowercase();
    builtin().into_iter().find(|e| e.sha1 == needle)
}

pub fn is_known_bios_sha1(sha1: &str) -> bool {
    const KNOWN: &[&str] = &[
        "300c20df6731a33952ded8c436f7f186d25d3492",
        "c11531d5261006810cdc954bd4bec0afe3187b35",
    ];
    let needle = sha1.to_ascii_lowercase();
    KNOWN.iter().any(|k| *k == needle)
}

pub fn cover_urls(game_id: &str) -> Option<(String, String)> {
    // Curated direct file URLs (tested). Old en.wikipedia fair-use paths 404;
    // Cover Project CDN returns 403 to non-browser clients — keep as documented fallback.
    match game_id {
        "ruby-usa" => Some((
            "https://upload.wikimedia.org/wikipedia/en/b/be/PokemonRubySapphireBox.jpg".into(),
            "https://www.thecoverproject.net/images/covers/gbapkmnrubyusa.jpg".into(),
        )),
        "sapphire-usa" | "sapphire-usa-rev1" => Some((
            "https://upload.wikimedia.org/wikipedia/en/b/be/PokemonRubySapphireBox.jpg".into(),
            "https://www.thecoverproject.net/images/covers/gbapkmnsapphireusa.jpg".into(),
        )),
        "emerald-usa" => Some((
            // PT Wikipedia file page: Ficheiro:Pokémon_Emerald_cover.png
            "https://upload.wikimedia.org/wikipedia/pt/7/72/Pok%C3%A9mon_Emerald_cover.png".into(),
            "https://www.thecoverproject.net/images/covers/gbapkmnemeraldusa.jpg".into(),
        )),
        _ => None,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn emerald_sha1_is_catalogued() {
        let e = find_by_sha1("f3ae088181bf583e55daf962a92bb46f4f1d07b7").expect("emerald");
        assert_eq!(e.id, "emerald-usa");
    }

    #[test]
    fn unknown_sha1_is_rejected() {
        assert!(find_by_sha1("0000000000000000000000000000000000000000").is_none());
    }

    #[test]
    fn cover_urls_exist_for_mvp() {
        assert!(cover_urls("emerald-usa").is_some());
        assert!(cover_urls("not-a-game").is_none());
    }

    #[test]
    fn known_bios_sha1() {
        assert!(is_known_bios_sha1("300c20df6731a33952ded8c436f7f186d25d3492"));
        assert!(!is_known_bios_sha1("deadbeef"));
    }
}
