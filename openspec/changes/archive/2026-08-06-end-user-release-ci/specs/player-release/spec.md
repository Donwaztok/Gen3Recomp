## Purpose

Defines how end-user player packages are built and published: GitHub Actions produces multi-OS archives that include the host, launcher, empty ROM folder placeholder, and docs, then attaches them to GitHub Releases without illegal payloads.

## ADDED Requirements

### Requirement: Tag-triggered GitHub Release artifacts
Pushing a version tag that matches the project's documented release tag pattern MUST trigger a GitHub Actions workflow that builds player packages and publishes them as assets on a GitHub Release for that tag.

#### Scenario: Version tag creates a Release
- **GIVEN** a tagged commit that matches the documented release tag pattern
- **WHEN** the release workflow completes successfully
- **THEN** a GitHub Release exists for that tag with downloadable player package assets

### Requirement: Multi-OS player packages
The release workflow MUST produce separate player packages for Linux, Windows, and macOS. Each package MUST include both the gameplay host binary and the Tauri launcher binary (or platform-equivalent player entrypoint that launches the UI).

#### Scenario: Three platform assets
- **WHEN** a successful multi-OS release completes
- **THEN** the Release lists distinct Linux, Windows, and macOS player packages, and each package contains the host and the launcher

### Requirement: Package layout includes empty roms directory
Each player package MUST include an empty `roms/` directory (or equivalent placeholder with a short README) at the documented package root so users know where to place catalogued dumps. The package MUST NOT include `.gba` dumps or BIOS images.

#### Scenario: Fresh unzip shows roms placeholder
- **WHEN** a user extracts a Release player package
- **THEN** a `roms/` folder is present for user dumps and contains no Nintendo ROM or BIOS files

### Requirement: Release packages remain D7-safe
Published Release assets MUST NOT contain ROM dumps, GBA BIOS images, cover image blobs, or prebuilt cart AOT objects.

#### Scenario: Asset content audit
- **WHEN** a maintainer or automated check inspects a published player package
- **THEN** no `.gba`, BIOS image, cover blob, or cart AOT library is present inside the archive
