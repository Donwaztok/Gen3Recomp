## 1. Cover action overlay

- [x] 1.1 Restructure grid tile markup so the cover action is not a button nested inside another button (select vs action)
- [x] 1.2 Overlay icon-only Play on covers with `aot_ready` (respect BIOS/host/`busy` gates; accessible label)
- [x] 1.3 Overlay icon-only Build on covers without `aot_ready` (accessible label; triggers build for that ROM)
- [x] 1.4 Style overlay (position on cover, contrast, hover/focus); keep footer Play/Build as secondary

## 2. Validation

- [x] 2.1 Manual: AOT-ready Emerald shows Play icon; click starts host
- [x] 2.2 Manual: missing-AOT title shows Build icon; click starts build
- [x] 2.3 Manual: click outside icon still selects the tile
