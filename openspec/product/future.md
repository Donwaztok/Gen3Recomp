# Future vision (parked)

These items are real, but they are **not** MVP work. Do not create modules, interfaces, or tasks for them now.

## Near-future after MVP

- Pokémon FireRed and LeafGreen as additional Game Definitions
- EU / JP revisions as extra catalog rows
- Host-clock RTC option (upstream already experiments with this)
- Desktop packaging (installers, Flatpak, etc.)
- macOS / Windows CI in addition to Linux

## Later product ideas

| Idea | Why parked |
|------|------------|
| Multiplayer / trading / battle link | Needs serial/link cable model + netcode |
| Vulkan | SDL3 + framebuffer blit is enough; GPU API is a presentation upgrade |
| Mods | Upstream `.gbamod` is large product surface |
| Save states | Host feature; easy to want, easy to derail boot work |
| Plugin system | ABI and security cost without MVP benefit |
| Editor / debugger UI | Different product |
| Networking | Same family as multiplayer |
| Adaptive widescreen | Game-aware presentation; violates “host knows nothing about Pokémon” unless carefully designed later |
| Android | Upstream can do it; we are desktop-first |

## Rule

If a change proposal starts implementing anything in this file, stop and split it out of MVP.
