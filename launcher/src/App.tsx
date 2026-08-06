import { useCallback, useEffect, useMemo, useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import { open } from "@tauri-apps/plugin-dialog";
import { Button, Chip, Modal, Spinner, Switch } from "@heroui/react";

type RomEntry = {
  path: string;
  sha1: string;
  game_id: string;
  display_name: string;
  region: string;
  aot_ready: boolean;
  cover_data_url: string | null;
};

type BiosStatus = {
  present: boolean;
  path: string | null;
  valid: boolean;
  message: string;
};

type ModInfo = {
  id: string;
  name: string;
  version: string;
  enabled: boolean;
  valid: boolean;
  invalid_reason: string;
};

type LibraryState = {
  roms: RomEntry[];
  bios: BiosStatus;
  mods: ModInfo[];
  repo_root: string | null;
  host_path: string | null;
  message: string;
};

function App() {
  const [library, setLibrary] = useState<LibraryState | null>(null);
  const [selectedSha1, setSelectedSha1] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);
  const [status, setStatus] = useState("Loading library…");
  const [modsOpen, setModsOpen] = useState(false);
  const [errorOpen, setErrorOpen] = useState(false);
  const [errorText, setErrorText] = useState("");

  const showError = (text: string) => {
    setErrorText(text);
    setErrorOpen(true);
  };

  const refresh = useCallback(async (fetchCovers: boolean) => {
    setBusy(true);
    try {
      const next = await invoke<LibraryState>("refresh_library", {
        fetchCovers,
      });
      setLibrary(next);
      setSelectedSha1((prev) => {
        if (prev && next.roms.some((r) => r.sha1 === prev)) return prev;
        return next.roms[0]?.sha1 ?? null;
      });
      setStatus(
        next.roms.length === 0
          ? "Drop catalogued dumps in roms/ or use Add ROM"
          : `${next.roms.length} title(s) · covers cached locally (D7)`,
      );
      // Background cover fetch for missing tiles
      for (const rom of next.roms) {
        if (!rom.cover_data_url) {
          void invoke<string | null>("fetch_cover", { gameId: rom.game_id })
            .then((url) => {
              if (!url) return;
              setLibrary((cur) => {
                if (!cur) return cur;
                return {
                  ...cur,
                  roms: cur.roms.map((r) =>
                    r.game_id === rom.game_id
                      ? { ...r, cover_data_url: url }
                      : r,
                  ),
                };
              });
            })
            .catch(() => undefined);
        }
      }
    } catch (e) {
      showError(String(e));
    } finally {
      setBusy(false);
    }
  }, []);

  useEffect(() => {
    void refresh(true);
  }, [refresh]);

  const onRefresh = async () => {
    setStatus("Refreshing covers…");
    await refresh(true);
  };

  const selected = useMemo(
    () => library?.roms.find((r) => r.sha1 === selectedSha1) ?? null,
    [library, selectedSha1],
  );

  const canPlay =
    !!selected &&
    !!library?.bios.valid &&
    selected.aot_ready &&
    !!library.host_path;

  const onAddRom = async () => {
    const picked = await open({
      multiple: false,
      filters: [{ name: "GBA ROM", extensions: ["gba"] }],
    });
    if (!picked || Array.isArray(picked)) return;
    setBusy(true);
    try {
      const entry = await invoke<RomEntry>("identify_rom", { path: picked });
      setStatus(`Added ${entry.display_name}`);
      await refresh(false);
      setSelectedSha1(entry.sha1);
    } catch (e) {
      showError(String(e));
    } finally {
      setBusy(false);
    }
  };

  const onBuild = async () => {
    if (!selected) return;
    setBusy(true);
    setStatus("Building cart AOT — this can take several minutes…");
    try {
      const log = await invoke<string>("build_cart", {
        romPath: selected.path,
      });
      setStatus(log.split("\n").filter(Boolean).slice(-1)[0] ?? "Build done");
      await refresh(false);
    } catch (e) {
      showError(String(e));
      setStatus("Build failed");
    } finally {
      setBusy(false);
    }
  };

  const onPlay = async () => {
    if (!selected || !library?.bios.path) return;
    setBusy(true);
    try {
      await invoke("play_rom", {
        romPath: selected.path,
        biosPath: library.bios.path,
      });
      setStatus(`Started ${selected.display_name}`);
    } catch (e) {
      showError(String(e));
    } finally {
      setBusy(false);
    }
  };

  const onToggleMod = async (id: string, enabled: boolean) => {
    try {
      await invoke("set_mod_enabled", { id, enabled });
      await refresh(false);
    } catch (e) {
      showError(String(e));
    }
  };

  return (
    <div className="g3-shell">
      <header>
        <h1 className="g3-brand">
          Gen3<span className="g3-brand-accent">Recomp</span>
        </h1>
        <p className="g3-tagline">
          Your catalogued Gen3 dumps. Build once, play natively — covers stay on
          your machine.
        </p>
      </header>

      {!library || library.roms.length === 0 ? (
        <div className="g3-empty">
          <p>No catalogued ROMs in roms/ yet.</p>
          <p>USA Ruby, Sapphire, or Emerald — then Add ROM.</p>
          <div className="mt-4">
            <Button onPress={() => void onAddRom()} isDisabled={busy}>
              Add ROM…
            </Button>
          </div>
        </div>
      ) : (
        <div className="g3-grid">
          {library.roms.map((rom, i) => (
            <button
              type="button"
              key={rom.sha1}
              className={`g3-tile${selectedSha1 === rom.sha1 ? " is-selected" : ""}`}
              style={{ animationDelay: `${0.04 * i}s` }}
              onClick={() => setSelectedSha1(rom.sha1)}
            >
              <div
                className={`g3-cover g3-cover--${rom.game_id.replace(/[^a-z0-9-]/gi, "")}`}
              >
                {rom.cover_data_url ? (
                  <img src={rom.cover_data_url} alt="" draggable={false} />
                ) : (
                  <span className="g3-placeholder">{rom.display_name}</span>
                )}
              </div>
              <div className="g3-tile-meta">
                <p className="g3-tile-title">{rom.display_name}</p>
                <p className="g3-tile-sub">
                  {rom.region}
                  {" · "}
                  {rom.aot_ready ? "Ready" : "Needs Build"}
                </p>
              </div>
            </button>
          ))}
        </div>
      )}

      <footer className="g3-footer">
        <div className="g3-status">
          <div className="flex flex-wrap gap-2 mb-1">
            <Chip size="sm" variant={library?.bios.valid ? "primary" : "soft"}>
              {library?.bios.valid ? "BIOS OK" : "BIOS needed"}
            </Chip>
            <Chip
              size="sm"
              variant={selected?.aot_ready ? "primary" : "soft"}
            >
              {selected
                ? selected.aot_ready
                  ? "AOT ready"
                  : "AOT missing"
                : "No selection"}
            </Chip>
            {!library?.host_path && (
              <Chip size="sm" variant="soft">
                Host missing
              </Chip>
            )}
            {busy && <Spinner size="sm" />}
          </div>
          <div>{status}</div>
          {library?.bios.message && (
            <div>{library.bios.message}</div>
          )}
        </div>
        <div className="g3-actions">
          <Button variant="ghost" onPress={() => void onRefresh()} isDisabled={busy}>
            Refresh
          </Button>
          <Button variant="secondary" onPress={() => void onAddRom()} isDisabled={busy}>
            Add ROM…
          </Button>
          <Button variant="secondary" onPress={() => setModsOpen(true)}>
            Mods
          </Button>
          <Button
            variant="outline"
            onPress={() => void onBuild()}
            isDisabled={busy || !selected}
          >
            Build
          </Button>
          <Button onPress={() => void onPlay()} isDisabled={busy || !canPlay}>
            Play
          </Button>
        </div>
      </footer>

      <Modal isOpen={modsOpen} onOpenChange={setModsOpen}>
        <Modal.Backdrop>
          <Modal.Container>
            <Modal.Dialog className="sm:max-w-[420px]">
              <Modal.CloseTrigger />
              <Modal.Header>
                <Modal.Heading>Mods</Modal.Heading>
              </Modal.Header>
              <Modal.Body>
                {!library?.mods.length ? (
                  <p className="text-sm opacity-70">
                    No mods — place packages in mods/&lt;id&gt;/mod.toml
                  </p>
                ) : (
                  library.mods.map((mod) => (
                    <div className="g3-mod-row" key={mod.id}>
                      <div>
                        <div className="font-medium">
                          {mod.name}{" "}
                          <span className="opacity-60 text-sm">v{mod.version}</span>
                        </div>
                        {!mod.valid && (
                          <div className="text-sm text-danger">
                            {mod.invalid_reason || "Invalid package"}
                          </div>
                        )}
                      </div>
                      <Switch
                        aria-label={`Toggle ${mod.name}`}
                        isSelected={mod.enabled}
                        isDisabled={!mod.valid}
                        onChange={(v) => void onToggleMod(mod.id, v)}
                      >
                        <Switch.Content>
                          <Switch.Control>
                            <Switch.Thumb />
                          </Switch.Control>
                        </Switch.Content>
                      </Switch>
                    </div>
                  ))
                )}
              </Modal.Body>
              <Modal.Footer>
                <Button variant="secondary" onPress={() => setModsOpen(false)}>
                  Close
                </Button>
              </Modal.Footer>
            </Modal.Dialog>
          </Modal.Container>
        </Modal.Backdrop>
      </Modal>

      <Modal isOpen={errorOpen} onOpenChange={setErrorOpen}>
        <Modal.Backdrop>
          <Modal.Container>
            <Modal.Dialog className="sm:max-w-[440px]">
              <Modal.CloseTrigger />
              <Modal.Header>
                <Modal.Heading>Something went wrong</Modal.Heading>
              </Modal.Header>
              <Modal.Body>
                <pre className="whitespace-pre-wrap text-sm opacity-80">
                  {errorText}
                </pre>
              </Modal.Body>
              <Modal.Footer>
                <Button onPress={() => setErrorOpen(false)}>OK</Button>
              </Modal.Footer>
            </Modal.Dialog>
          </Modal.Container>
        </Modal.Backdrop>
      </Modal>
    </div>
  );
}

export default App;
