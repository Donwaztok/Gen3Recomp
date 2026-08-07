## 1. Measure repeated Play cost

- [x] 1.1 Add or enable phase timing logs for cart artifact activation ms, heal warm-load count/ms (if any), and time to first guest frame / BIOS intro
- [x] 1.2 Capture two Play runs on Emerald with ready AOT + existing heal cache; record which phase dominates the multi-minute stall

## 2. Heal-cache warm path

- [x] 2.1 Change default player path so prepare does not eagerly warm-load every overlay shard before guest execution (on-demand / miss load from valid cache files)
- [x] 2.2 Keep an explicit diagnostic eager-warm mode (env or equivalent) and document it
- [x] 2.3 Verify healed PCs still resolve from cache without recompile when shards are valid

## 3. Cart artifact activation

- [x] 3.1 Revisit `dlopen` flags / activation strategy for `libcart.so` to reduce repeated Play cost; keep static coverage correct
- [x] 3.2 Log cart-activation duration; confirm it is no longer a minute-scale stall alone

## 4. Launcher / docs

- [x] 4.1 Adjust launcher status copy so Build is the multi-minute one-time step and Play is not described as another multi-minute compile when AOT is ready
- [x] 4.2 Note in player docs the distinction between empty heal cache (possible first hitch) and warm repeated Play

## 5. Validation

- [x] 5.1 Smoke: Emerald with ready AOT + warm heal cache — two consecutive Plays, each typically under ~5s to early boot on reference Linux SSD
- [x] 5.2 Smoke: Play still reaches title / early boot correctly; no cart rebuild on Play
- [x] 5.3 Confirm D7: no cart/heal blobs added to git or Releases
