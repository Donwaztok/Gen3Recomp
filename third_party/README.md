# Third-party: gba-recomp

gen3recomp pins [gba-recomp](https://github.com/mstan/gbarecomp) at the revision in `gbarecomp.pin`.

Upstream license: PolyForm Noncommercial 1.0.0 (same family as this repository).

## Why it is pinned, not vendored blindly

gba-recomp is a full GBA recompiler + hardware runtime with its own SDL2 host loop (`gbarecomp::run_game`). gen3recomp owns SDL3 for the null/debug host path. Native boot wraps `run_game` inside `src/recomp/gba/` only: upstream presents video/audio/input until a steppable API exists. Do not include gba-recomp headers from other modules.

## Populate the pin

```sh
cmake -S . -B build -DGEN3RECOMP_FETCH_GBARECOMP=ON
```

or:

```sh
git clone https://github.com/mstan/gbarecomp.git third_party/gbarecomp
git -C third_party/gbarecomp checkout 2952aff2bb42f49de5903acf22af8fea3e2e3dee
```

The checkout is gitignored. Do not commit upstream sources.

## Upgrade

1. Update `gbarecomp.pin`.
2. Refresh the checkout.
3. Adapt only `src/recomp/gba/`.
4. Land that work as its own OpenSpec change.
