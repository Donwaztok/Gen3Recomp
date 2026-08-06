import { useCallback, useEffect, useMemo, useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";
import { open } from "@tauri-apps/plugin-dialog";
import { Button, Chip, Label, Modal, ProgressBar, Spinner, Switch } from "@heroui/react";

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

type BuildProgress = {
  sha1: string;
  phase: string;
  current: number;
  total: number;
  percent: number;
  message: string;
};

function IconPlay() {
  return (
    <svg viewBox="0 0 24 24" aria-hidden="true" className="g3-cover-icon">
      <path fill="currentColor" d="M8 5.5v13l11-6.5L8 5.5z" />
    </svg>
  );
}

function IconBuild() {
  return (
    <svg viewBox="0 0 24 24" aria-hidden="true" className="g3-cover-icon">
      <path
        fill="currentColor"
        d="M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77a6 6 0 0 1-7.94 7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91a6 6 0 0 1 7.94-7.94l-3.76 3.76z"
      />
    </svg>
  );
}

function App() {
  const [library, setLibrary] = useState<LibraryState | null>(null);
  const [selectedSha1, setSelectedSha1] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);
  const [buildingSha1, setBuildingSha1] = useState<string | null>(null);
  const [buildProgress, setBuildProgress] = useState<BuildProgress | null>(
    null,
  );
  const [status, setStatus] = useState("Loading library…");
  const [modsOpen, setModsOpen] = useState(false);
  const [errorOpen, setErrorOpen] = useState(false);
  const [errorText, setErrorText] = useState("");

  const showError = (text: string) => {
    setErrorText(text);
    setErrorOpen(true);
  };

  useEffect(() => {
    let unlisten: (() => void) | undefined;
    void listen<BuildProgress>("build-progress", (event) => {
      const payload = event.payload;
      setBuildProgress(payload);
      setBuildingSha1(payload.sha1);
      if (payload.message) {
        setStatus(payload.message);
      }
    }).then((fn) => {
      unlisten = fn;
    });
    return () => {
      unlisten?.();
    };
  }, []);

  const refresh = useCallback(
    async (
      fetchCovers: boolean,
      options?: { lockUi?: boolean; preserveStatus?: boolean },
    ) => {
      const lockUi = options?.lockUi !== false;
      if (lockUi) setBusy(true);
      try {
        const next = await invoke<LibraryState>("refresh_library", {
          fetchCovers,
        });
        setLibrary(next);
        setSelectedSha1((prev) => {
          if (prev && next.roms.some((r) => r.sha1 === prev)) return prev;
          return next.roms[0]?.sha1 ?? null;
        });
        if (!options?.preserveStatus) {
          setStatus(
            next.roms.length === 0
              ? "Drop catalogued dumps in roms/ or use Add ROM"
              : `${next.roms.length} title(s) · covers cached locally (D7)`,
          );
        }
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
        if (lockUi) setBusy(false);
      }
    },
    [],
  );

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

  const buildingRom = useMemo(
    () => library?.roms.find((r) => r.sha1 === buildingSha1) ?? null,
    [library, buildingSha1],
  );

  const canPlayRom = (rom: RomEntry | null | undefined) =>
    !!rom &&
    !!library?.bios.valid &&
    rom.aot_ready &&
    !!library.host_path;

  const canPlay = canPlayRom(selected);
  const buildInFlight = buildingSha1 !== null;

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
      await refresh(false, { lockUi: false });
      setSelectedSha1(entry.sha1);
    } catch (e) {
      showError(String(e));
    } finally {
      setBusy(false);
    }
  };

  const onBuild = async (rom?: RomEntry | null) => {
    const target = rom ?? selected;
    if (!target) return;
    if (buildingSha1) {
      showError(
        "A cart AOT Build is already running. Wait for it to finish before starting another.",
      );
      return;
    }
    setSelectedSha1(target.sha1);
    setBuildingSha1(target.sha1);
    setBuildProgress({
      sha1: target.sha1,
      phase: "generate",
      current: 0,
      total: 1,
      percent: 0,
      message: `Building ${target.display_name} — one-time step, can take several minutes…`,
    });
    setStatus(
      `Building ${target.display_name} — one-time step, can take several minutes…`,
    );
    await new Promise<void>((resolve) => {
      requestAnimationFrame(() => requestAnimationFrame(() => resolve()));
    });
    try {
      const log = await invoke<string>("build_cart", {
        romPath: target.path,
      });
      setStatus(
        log.split("\n").filter(Boolean).slice(-1)[0] ??
          `${target.display_name} ready — Play should open quickly now`,
      );
      await refresh(false, { lockUi: false, preserveStatus: true });
    } catch (e) {
      showError(String(e));
      setStatus("Build failed");
    } finally {
      setBuildingSha1(null);
      setBuildProgress(null);
    }
  };

  const onPlay = async (rom?: RomEntry | null) => {
    const target = rom ?? selected;
    if (!target || !library?.bios.path) return;
    setSelectedSha1(target.sha1);
    setBusy(true);
    setStatus(`Starting ${target.display_name}…`);
    try {
      await invoke("play_rom", {
        romPath: target.path,
        biosPath: library.bios.path,
      });
      setStatus(`Started ${target.display_name}`);
    } catch (e) {
      showError(String(e));
    } finally {
      setBusy(false);
    }
  };

  const onToggleMod = async (id: string, enabled: boolean) => {
    try {
      await invoke("set_mod_enabled", { id, enabled });
      await refresh(false, { lockUi: false });
    } catch (e) {
      showError(String(e));
    }
  };

  const coverProgressFor = (sha1: string) =>
    buildingSha1 === sha1 ? buildProgress : null;

  return (
    <div className="g3-shell">
      <header>
        <h1 className="g3-brand">
          Gen3<span className="g3-brand-accent">Recomp</span>
        </h1>
        <p className="g3-tagline">
          Build cart AOT once (can take minutes). After that, Play should open
          quickly — covers stay on your machine.
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
          {library.roms.map((rom, i) => {
            const tileCanPlay = canPlayRom(rom);
            const tileProgress = coverProgressFor(rom.sha1);
            const isBuildingThis = buildingSha1 === rom.sha1;
            return (
              <div
                key={rom.sha1}
                className={`g3-tile${selectedSha1 === rom.sha1 ? " is-selected" : ""}${isBuildingThis ? " is-building" : ""}`}
                style={{ animationDelay: `${0.04 * i}s` }}
              >
                <div
                  className={`g3-cover g3-cover--${rom.game_id.replace(/[^a-z0-9-]/gi, "")}`}
                  role="button"
                  tabIndex={0}
                  aria-pressed={selectedSha1 === rom.sha1}
                  aria-label={`Select ${rom.display_name}`}
                  onClick={() => setSelectedSha1(rom.sha1)}
                  onKeyDown={(e) => {
                    if (e.key === "Enter" || e.key === " ") {
                      e.preventDefault();
                      setSelectedSha1(rom.sha1);
                    }
                  }}
                >
                  {rom.cover_data_url ? (
                    <img src={rom.cover_data_url} alt="" draggable={false} />
                  ) : (
                    <span className="g3-placeholder">{rom.display_name}</span>
                  )}
                  {tileProgress && (
                    <div
                      className="g3-cover-progress"
                      onClick={(e) => e.stopPropagation()}
                      onKeyDown={(e) => e.stopPropagation()}
                    >
                      <ProgressBar
                        aria-label={`Building ${rom.display_name}`}
                        className="w-full"
                        color="accent"
                        size="sm"
                        maxValue={100}
                        value={Math.round(tileProgress.percent ?? 0)}
                        formatOptions={{
                          style: "decimal",
                          maximumFractionDigits: 0,
                        }}
                        isIndeterminate={
                          tileProgress.phase === "generate" &&
                          tileProgress.current === 0
                        }
                      >
                        <Label className="g3-cover-progress-label">
                          {tileProgress.message || "Building…"}
                        </Label>
                        <ProgressBar.Track>
                          <ProgressBar.Fill />
                        </ProgressBar.Track>
                      </ProgressBar>
                    </div>
                  )}
                  {!isBuildingThis && (
                    <div className="g3-cover-action">
                      {rom.aot_ready ? (
                        <button
                          type="button"
                          className="g3-cover-btn"
                          aria-label={`Play ${rom.display_name}`}
                          title="Play"
                          disabled={busy || !tileCanPlay}
                          onClick={(e) => {
                            e.stopPropagation();
                            void onPlay(rom);
                          }}
                        >
                          <IconPlay />
                        </button>
                      ) : (
                        <button
                          type="button"
                          className="g3-cover-btn g3-cover-btn--build"
                          aria-label={`Build ${rom.display_name}`}
                          title="Build cart AOT"
                          disabled={busy || buildInFlight}
                          onClick={(e) => {
                            e.stopPropagation();
                            void onBuild(rom);
                          }}
                        >
                          <IconBuild />
                        </button>
                      )}
                    </div>
                  )}
                </div>
                <div className="g3-tile-meta">
                  <p className="g3-tile-title">{rom.display_name}</p>
                  <p className="g3-tile-sub">
                    {rom.region}
                    {" · "}
                    {isBuildingThis
                      ? "Building…"
                      : rom.aot_ready
                        ? "Ready"
                        : "Needs Build"}
                  </p>
                </div>
              </div>
            );
          })}
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
                  ? "AOT ready — Play opens quickly"
                  : "Needs Build (minutes, once)"
                : "No selection"}
            </Chip>
            {buildInFlight && (
              <Chip size="sm" variant="soft">
                Building {buildingRom?.display_name ?? "…"}
              </Chip>
            )}
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
            isDisabled={
              busy ||
              buildInFlight ||
              !selected ||
              !!selected?.aot_ready
            }
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
