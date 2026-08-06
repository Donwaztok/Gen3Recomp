## ADDED Requirements

### Requirement: Product brand reads Gen3Recomp
The Tauri launcher hero brand, HTML document title, and native window title MUST present the product name as **Gen3Recomp** (PascalCase compound). Lowercase `gen3recomp` MUST NOT be the primary visible brand string in those surfaces.

#### Scenario: First viewport brand
- **WHEN** the launcher window opens
- **THEN** the hero brand text reads Gen3Recomp (accent styling on the Recomp segment is allowed)

#### Scenario: OS window title
- **WHEN** the launcher window is shown in the desktop environment
- **THEN** the window title is Gen3Recomp

### Requirement: Brand uses a modern sans display face
The hero brand MUST use a modern sans-serif display typeface appropriate to the dark launcher shell. The previous serif brand face (Fraunces) MUST NOT remain the brand font.

#### Scenario: Brand is not serif Fraunces
- **WHEN** a contributor inspects the launcher stylesheet for the brand class
- **THEN** Fraunces is not the brand `font-family`, and a sans display face is applied instead
