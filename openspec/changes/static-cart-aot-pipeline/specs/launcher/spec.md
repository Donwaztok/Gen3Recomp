## ADDED Requirements

### Requirement: Default BIOS path beside working directory
When `--bios` is omitted, the launcher MUST look for `gba_bios.bin` in the current working directory before failing BIOS validation. It MUST NOT invent or download a BIOS.

#### Scenario: Local gba_bios.bin is used
- **GIVEN** `./gba_bios.bin` exists and is a valid catalogued BIOS
- **WHEN** the user runs `gen3recomp --rom <path>` without `--bios`
- **THEN** startup uses that BIOS file

### Requirement: No silent first-file ROM autoplay
The launcher MUST NOT start a session by silently picking the first `.gba` file in a directory. If multiple catalogued dumps are presented in one selection flow, the user MUST choose explicitly.

#### Scenario: Ambiguous multi-dump choice
- **GIVEN** more than one catalogued MVP dump is available to a selection UI or helper
- **WHEN** the user has not chosen among them
- **THEN** the application does not start a game session until one dump is selected or `--rom` is supplied

### Requirement: Prepare is not the primary warm path
`--prepare` (if retained) MUST be documented as optional/diagnostic. Help and README MUST prioritize local cart AOT for full-speed play.

#### Scenario: Help mentions AOT over prepare
- **WHEN** the user runs `gen3recomp --help`
- **THEN** usage still documents `--rom`, `--bios`, and `--version`
- **THEN** if `--prepare` appears, it is not presented as the required path to full-speed gameplay

## MODIFIED Requirements

### Requirement: Help text
`gen3recomp --help` MUST print usage that mentions `--rom`, `--bios`, and `--version`, then exit successfully. Usage MAY mention optional `--prepare` only as a secondary/diagnostic option and MUST NOT present it as the way to obtain full-cart native speed.

#### Scenario: Help invocation
- **WHEN** the user runs `gen3recomp --help`
- **THEN** usage text is printed and the process exits with status 0
- **THEN** `--rom` and `--bios` are documented
