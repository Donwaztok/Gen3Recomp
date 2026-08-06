## ADDED Requirements

### Requirement: Documented CI release path
Project documentation MUST describe how GitHub Actions builds and publishes multi-OS player packages to GitHub Releases, including the tag pattern that triggers a release and where artifacts appear.

#### Scenario: Reader finds release instructions
- **WHEN** a maintainer opens the project build or release documentation
- **THEN** they can identify the tag trigger, the three OS targets, and that Releases are the download location for end users

### Requirement: Packaging scripts support Linux, Windows, and macOS
The repository MUST provide packaging entrypoints (scripts and/or CI job steps) that assemble a D7-safe player package for Linux, Windows, and macOS, each including host + launcher + empty `roms/` + docs.

#### Scenario: Local packaging still works for Linux
- **WHEN** a maintainer runs the documented local packaging command on Linux
- **THEN** a player archive is produced under the documented `dist/` location with host, launcher, and empty `roms/`

#### Scenario: CI packages Windows and macOS
- **WHEN** the release workflow runs on Windows and macOS runners
- **THEN** each produces a platform player package suitable for upload to the GitHub Release

## MODIFIED Requirements

### Requirement: Player packaging excludes illegal payloads
Any documented player package that includes the Tauri launcher and/or `gen3recomp` MUST NOT include ROM dumps, BIOS images, cover image blobs, or prebuilt cart AOT objects. Automated Release packaging MUST apply the same exclusion.

#### Scenario: Release checklist
- **WHEN** a maintainer prepares a Release zip per project docs, or CI publishes a Release asset
- **THEN** the package contains host/launcher/tools and an empty `roms/` placeholder only — no dumps, BIOS, covers, or cart artifacts
