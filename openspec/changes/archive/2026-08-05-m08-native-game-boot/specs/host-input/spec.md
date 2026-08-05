## Purpose

Maps ordinary desktop controls to GBA buttons without exposing cheats, editors, or save-state controls in MVP.

## ADDED Requirements

### Requirement: Keyboard mapping
The input module MUST map a documented keyboard layout to the GBA d-pad, A, B, Start, Select, L, and R.

#### Scenario: Default keyboard layout
- **WHEN** the user presses a documented key for A
- **THEN** the session backend receives the GBA A button pressed state

### Requirement: Keyboard remains available if a gamepad is used
If gamepad mapping is implemented, it MUST target the same GBA buttons and MUST NOT disable the documented keyboard layout.

#### Scenario: Keyboard still works with gamepad present
- **WHEN** a gamepad is connected
- **THEN** documented keyboard controls continue to work

### Requirement: No save-state controls in MVP
The MVP input surface MUST NOT document or implement save-state hotkeys.

#### Scenario: No F-key save states
- **WHEN** a user reads MVP controls documentation
- **THEN** save states are not offered
