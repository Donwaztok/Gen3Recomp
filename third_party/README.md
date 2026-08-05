# Third-party: gba-recomp

gen3recomp tracks [gba-recomp](https://github.com/mstan/gbarecomp) as a **git submodule** at `third_party/gbarecomp`, pinned to the revision in `gbarecomp.pin`.

Upstream license: PolyForm Noncommercial 1.0.0 (same family as this repository).

## Why a submodule

gba-recomp is a full GBA recompiler + hardware runtime with its own SDL2 host loop (`gbarecomp::run_game`). gen3recomp owns SDL3 for the null/debug host path. Native boot wraps `run_game` inside `src/recomp/gba/` only. Do not include gba-recomp headers from other modules.

A submodule keeps the pin explicit, cloneable with the repo, and easy to bump without FetchContent side effects.

## Clone / update

```sh
git clone --recurse-submodules <this-repo>
# or, in an existing checkout:
git submodule update --init --recursive
```

## Upgrade

1. `git -C third_party/gbarecomp fetch origin`
2. `git -C third_party/gbarecomp checkout <new-revision>`
3. Update `gbarecomp.pin` to match.
4. Adapt only `src/recomp/gba/` if the upstream API moved.
5. Commit the submodule pointer (and pin) as its own change.
