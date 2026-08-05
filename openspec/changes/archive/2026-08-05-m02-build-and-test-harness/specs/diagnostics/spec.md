## Purpose

Provides a single logging setup so startup, validation, and provider failures can be reported consistently in later milestones.

## ADDED Requirements

### Requirement: Application logging is available
The product application MUST initialize a process-wide logger that can emit informational and error messages to stderr or a documented log sink.

#### Scenario: Log line on startup
- **WHEN** `gen3recomp` starts
- **THEN** it can emit at least one informational log or stdout identity line without crashing

### Requirement: Errors are loggable
Core error reporting MUST be able to log a failure message suitable for humans.

#### Scenario: Diagnostic error path
- **WHEN** a later subsystem reports a startup failure through core diagnostics
- **THEN** an error message can be written through the logging setup
