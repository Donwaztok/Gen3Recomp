## Purpose

Records durable engineering constraints so incremental implementation cannot quietly invent new modules, title-specific runtimes, or leaky backend integrations.

## Requirements

### Requirement: Single product executable
The build MUST produce exactly one product application named `gen3recomp`. A separate test runner is allowed.

#### Scenario: Product target
- **WHEN** the release build completes
- **THEN** the product output is a single `gen3recomp` application and not one binary per game title

### Requirement: Title logic stays in Game Definitions
Runtime, platform, video, audio, and input MUST NOT branch on Pokémon title identity. Title differences MUST be expressed as Game Definition data consumed at identification or provider prepare time.

#### Scenario: No Emerald special case in runtime
- **WHEN** a contributor inspects runtime and host presentation modules
- **THEN** those modules contain no Ruby, Sapphire, or Emerald specific behavior

### Requirement: Isolated recompiler backend
Only the dedicated gba adapter area MAY depend on gba-recomp headers, types, or tools. All other modules MUST use gen3recomp provider/session contracts.

#### Scenario: Host modules stay backend-agnostic
- **WHEN** a second provider is considered later
- **THEN** host modules outside the adapter folder do not require edits solely because upstream types changed

### Requirement: Game-agnostic presentation
Platform, video, audio, and input MUST present only generic host or GBA primitives such as windows, framebuffers, PCM samples, and button states.

#### Scenario: Renderer does not know Pokémon
- **WHEN** video presents a frame
- **THEN** it does so without using Pokémon game concepts, names, or assets

### Requirement: Incremental runnable slices
Each archived milestone change MUST leave the software executable and testable. Later milestones MUST NOT be required for earlier ones to build.

#### Scenario: M01 is useful alone
- **WHEN** only the project skeleton milestone has been implemented
- **THEN** a developer can configure, build, and run `gen3recomp` to observe a trivial but real program output
