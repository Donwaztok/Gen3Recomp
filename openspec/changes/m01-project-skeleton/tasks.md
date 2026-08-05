## 1. Repository hygiene

- [ ] 1.1 Add `.gitignore` for build trees, IDE junk, `*.gba`, BIOS binaries, and generated recomp output directories
- [ ] 1.2 Add `LICENSE` as PolyForm Noncommercial 1.0.0 (D11) and keep the README license section accurate
- [ ] 1.3 Add a short `CONTRIBUTING.md` pointing at `openspec/README.md` and the milestone workflow

## 2. CMake skeleton

- [ ] 2.1 Create root `CMakeLists.txt` (C++20, product target `gen3recomp`)
- [ ] 2.2 Create `src/main.cpp` and `src/app` (+ `src/core` if needed)
- [ ] 2.3 Support `gen3recomp --version` (a trivial argv check is enough) and wire module static libraries into the product executable

## 3. Docs and verification

- [ ] 3.1 Document configure/build/run commands in README
- [ ] 3.2 Configure, build, and run `gen3recomp --version` locally; confirm exit 0 and identity output
