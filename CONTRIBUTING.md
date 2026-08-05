# Contributing

This project is specified before it is implemented. Read [openspec/README.md](openspec/README.md) first.

## Workflow

1. Work on one OpenSpec change at a time (`openspec/changes/m0N-…`).
2. Implement only that milestone.
3. Leave a runnable, testable artifact.
4. Archive the change after it is done.
5. Then start the next milestone.

Do not pull later milestones forward. Parked ideas live in [openspec/product/future.md](openspec/product/future.md).

## Rules

- All code and repository prose are English.
- Do not commit ROMs, BIOS images, Nintendo assets, or ROM-derived generated sources.
- Runtime must not contain title-specific logic.
- Platform / video / audio / input must not know Pokémon.
- Only `src/recomp/gba/` may depend on gba-recomp.

## License

Contributions are accepted under the PolyForm Noncommercial License 1.0.0. See [LICENSE](LICENSE).
