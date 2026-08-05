# Decisions

Architecture Decision Records for the MVP. Newest first inside each group only when superseding. These are the places where a “clever” alternative was rejected.

## D1 — Few modules; fold Util and Filesystem

**Decision:** Modules are `core`, `app`, `game`, `recomp`, `runtime`, `platform`, `video`, `audio`, `input`. No `util`. No `filesystem`. Tests are a CMake target, not a product module.

**Why:** Util becomes a junk drawer. Filesystem in MVP is “read ROM/BIOS, write user data/cache/saves” and belongs in `core` (paths + bytes) plus `platform` (native dialogs).

**Rejected:** One module per noun in the first brainstorm, including Util/Filesystem/Tests.

## D2 — Host window before gba-recomp

**Decision:** Milestone order is launcher → validation → SDL3 window → runtime loop → provider → boot.

**Why:** The heaviest, most volatile dependency arrives only after the host shell is testable. Boot then becomes “plug a backend into an existing loop”.

**Rejected:** Integrating gba-recomp before any window exists. That piles provider bring-up, SDL, and boot debugging into one step.

## D3 — App is a thin launcher, not a GUI product

**Decision:** MVP launcher is CLI-first: `gen3recomp --rom <path> [--bios <path>]`. If ROM is omitted, use one native file dialog via SDL3. No ImGui/Qt launcher shell.

**Why:** The goal is native boot, not a frontend. A pretty launcher can come later without changing Runtime.

**Rejected:** Immediate multi-page launcher UI, cover art browser, or settings app.

## D4 — Do not reimplement the GBA PPU/APU

**Decision:** `video` / `audio` / `input` are host adapters. GBA hardware stays inside the provider runtime (gba-recomp).

**Why:** Rebuilding a PPU “because we have a Renderer module” is overengineering and fights upstream.

**Rejected:** A gen3recomp-owned tile/sprite renderer that bypasses upstream VRAM/OAM semantics.

## D5 — Game Definition data, shared Gen3 family

**Decision:** Titles are catalog rows. Shared behavior is a Gen3 family profile. Runtime has zero title switches.

**Why:** Ruby/Sapphire/Emerald share an engine. Duplicating runtimes (as three upstream repos do) is exactly what this project exists to avoid.

**Rejected:** `EmeraldRuntime` subclasses or `#ifdef EMERALD` in the host.

## D6 — Provider seam without a plugin system

**Decision:** Compile-time adapter behind two tiny contracts (prepare + session). One adapter in MVP.

**Why:** Plugins imply ABI, discovery, versioning, and security surface. We only need the ability to add a second adapter later by writing another folder and wiring it.

**Rejected:** dlopen plugins, abstract factory registries, dependency-injection containers.

## D7 — No shipped ROM-derived code

**Decision:** The distributed project and any public binary must not contain generated C++ lifted from a ROM. Preparation happens on the user’s machine from the user’s dump.

**Why:** Generated recomp output is derived from copyrighted machine code. Shipping it would violate the project’s legal stance.

**Rejected:** Build-time embedding of Ruby/Sapphire/Emerald recompiled sources into the public artifact (the classic single-game recomp repo model).

## D8 — USA revisions only in MVP

**Decision:** Catalog starts with one canonical USA dump per title: Ruby, Sapphire, Emerald.

**Why:** Identity is exact SHA-1. Supporting every region/revision immediately explodes validation and provider metadata.

**Rejected:** “Any Gen3 ROM header that looks close enough.”

## D9 — BIOS is required

**Decision:** A user-supplied GBA BIOS is part of startup. Validate it. Do not HLE-skip the BIOS boot path.

**Why:** gba-recomp’s correctness model executes BIOS code. The original prompt omitted BIOS; architecture cannot.

**Rejected:** Silent BIOS HLE to make first boot “easier”.

## D10 — One product exe; tests are a second target

**Decision:** Ship `gen3recomp`. Develop `gen3recomp_tests` via Catch2/CTest.

**Why:** “Only one executable” means one product, not “ban the test runner”.

**Rejected:** Multiple game binaries (`EmeraldRecomp`, `RubyRecomp`, …). Rejected: stuffing tests into the product binary.

## D11 — Same license as gba-recomp: PolyForm Noncommercial 1.0.0

**Decision:** The entire `gen3recomp` project uses **PolyForm Noncommercial License 1.0.0**, matching [gba-recomp](https://github.com/mstan/gbarecomp). Source stays public and contribution-friendly. Commercial use is not permitted. Do not market the project as OSI “open source”; call it a public noncommercial source-available preservation project.

**Why:** The host exists to recompile proprietary Game Boy Advance titles. The maintainer does not intend commercial use, and linking gba-recomp already imposes the same noncommercial limit. One license for host + backend is simpler than a MIT/NC split.

**Rejected:** MIT host + NOTICE split. Rejected: claiming unrestricted open-source/commercial rights on a Gen3 recomp host.

## D12 — English in the repository

**Decision:** All specs, product docs, code identifiers, and commit messages are English.

**Why:** Open-source contribution and upstream sync. Conversation with the current author can stay in Portuguese.

## D13 — Cartridge saves in, save states out

**Decision:** MVP includes whatever cartridge save/RTC behavior the provider already models, enough for the titles to function. Save states are future.

**Why:** “The game runs” includes being able to save in-game. Save states are a host feature with large scope (and upstream already has them — we still keep them out of *our* MVP surface).

## D14 — Pin upstream; sync in one folder

**Decision:** gba-recomp is a pinned git submodule at `third_party/gbarecomp`. All upstream churn is absorbed in that submodule + `src/recomp/gba`.

**Why:** Easy contribution and easy sync require a single integration wound, not gba includes sprinkled through Runtime/Platform.

## D15 — Framebuffer-first video

**Decision:** MVP video contract is “present this GBA framebuffer each frame”. Sprites/tiles/tilemaps/palettes/windows are allowed later as generic primitives, not as a Pokémon renderer.

**Why:** Matches what a hardware-faithful backend naturally emits. Designing a full 2D scene graph now is speculative.

## D16 — C++20 without a framework soup

**Decision:** C++20 + CMake + SDL3 + fmt + spdlog + Catch2. No extra architecture libraries (no ECS, no plugin SDK, no reflection codegen).

**Why:** The stack was chosen by the project. Adding more “help” now would fight simplicity.
