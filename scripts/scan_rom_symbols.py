#!/usr/bin/env python3
"""Emit gba_recompile --symbols TSV from prologue-shaped ROM bytes."""

from __future__ import annotations

import argparse
import pathlib
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom")
    parser.add_argument("-o", "--out", required=True)
    parser.add_argument("--base", default="0x08000000")
    parser.add_argument("--start", default="0x080000C0")
    parser.add_argument("--end", default="0x08B00000")
    args = parser.parse_args()

    rom = pathlib.Path(args.rom).read_bytes()
    base = int(args.base, 0)
    start = int(args.start, 0)
    end = int(args.end, 0)
    off0 = max(0, start - base)
    off1 = min(len(rom), end - base)

    rows: list[tuple[int, str]] = []
    off = off0 & ~1
    while off + 1 < off1:
        half = rom[off] | (rom[off + 1] << 8)
        if (half & 0xFF00) == 0xB500:
            rows.append((base + off, "thumb"))
        off += 2

    off = off0 & ~3
    while off + 3 < off1:
        word = (
            rom[off]
            | (rom[off + 1] << 8)
            | (rom[off + 2] << 16)
            | (rom[off + 3] << 24)
        )
        if (word & 0xFFFF4000) == 0xE92D4000:
            rows.append((base + off, "arm"))
        off += 4

    rows = sorted(set(rows))
    out = pathlib.Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("w", encoding="utf-8") as fh:
        fh.write("# addr\\tmode\\tname\n")
        for addr, mode in rows:
            prefix = "t" if mode == "thumb" else "a"
            fh.write(f"0x{addr:08X}\t{mode}\t{prefix}func_{addr:08X}\n")
    print(f"wrote {len(rows)} seeds to {out}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
