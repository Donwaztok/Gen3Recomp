# static-cart-aot Specification

## Purpose
Defines how a user-supplied catalogued GBA ROM is ahead-of-time recompiled into native code on the user’s machine so gameplay runs at full speed without shipping ROM-derived sources.
## Requirements
### Requirement: Official play path uses local cart AOT
For catalogued MVP titles, the documented and primary way to achieve full-speed native play MUST be local static recompilation of the user’s ROM (plus local BIOS recompilation as already required), not a bounded frame warm-up of the self-heal cache.

#### Scenario: Docs describe AOT first
- **WHEN** a developer follows README or manual-boot instructions for Emerald USA
- **THEN** the primary steps generate local cart AOT sources, rebuild or install an artifact, then launch with `--rom` / `--bios`
- **THEN** `--prepare` is absent from the primary path or clearly marked optional/diagnostic

### Requirement: AOT covers the catalogued cart code region
Local cart AOT for a catalogued dump MUST emit a dispatch table and native bodies covering the discovered executable cart region for that dump’s AOT config, not only entry/boot seeds.

#### Scenario: Emerald USA corpus is whole-cart scale
- **GIVEN** Emerald USA and the project Emerald AOT config
- **WHEN** the user runs the documented cart recompile script successfully
- **THEN** the output includes a non-empty dispatch table and multiple codegen shards under a gitignored local directory

### Requirement: ROM-derived AOT output stays local
Cart AOT sources and objects MUST remain outside version control. The repository MAY ship scripts, TOML configs, and CMake hooks that consume local output when present.

#### Scenario: Clean git tree
- **WHEN** a contributor regenerates cart AOT from their dump
- **THEN** `generated/rom/` (or the documented equivalent) stays gitignored and is not required for a clean clone to build the host stub path

### Requirement: Ruby and Sapphire share the same AOT pipeline
Catalogued Ruby USA and Sapphire USA MUST use the same host scripts and provider path as Emerald, with per-title AOT config data—not separate executables.

#### Scenario: Same tools, different config
- **GIVEN** a catalogued Ruby or Sapphire USA dump
- **WHEN** the user runs the cart recompile flow with the matching title config
- **THEN** local AOT output is produced and the same `gen3recomp` binary can launch that dump after prepare/rebuild

