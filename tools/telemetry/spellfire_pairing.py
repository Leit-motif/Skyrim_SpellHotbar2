"""Pair every accepted SpellFire event in a SpellHotbar2 log against its isolation line.

Ticket 61. Isolation and acceptance are the same question -- is this event a driver cast's own?
-- and they are logged from two adjacent sites in the animation-event hook:

    animationeventhook.cpp   SH2 cast: isolated {left,right}-hand caster before vanilla SpellFire
    casting_controller.cpp   SH2 cast: graph raised a {left,right} SpellFire event

Before ticket 61 only isolation carried the driver-active term, so a vanilla release in a hand
the LAST cast armed produced an accepted event with no isolation beside it. That orphan is the
defect's whole signature, and this script counts it.

An orphan is an accepted event with no isolation for the same hand inside `--window` ms before
it. Isolation is logged first, on the same millisecond in every healthy pair observed so far;
the window is slack, not an expectation.

    python spellfire_pairing.py <log> [--window-ms 60] [--since "2026-08-29 15:43:00"]

Exit status is 1 when any orphan is found, so a live acceptance cell can gate on it.
"""

from __future__ import annotations

import argparse
import re
import sys
from datetime import datetime

TS = r"\[(\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\.\d{3})\]"
ACCEPT = re.compile(TS + r".*graph raised a (left|right) SpellFire event")
ISOLATE = re.compile(TS + r".*isolated (left|right)-hand caster before vanilla SpellFire")
# The driver's own state edges, so an orphan can be reported with the cast context around it.
ENTRY = re.compile(TS + r".*notified (SH2_Cast\w*) \((?:clip (\d+)|held channel)\) -> (true|false)")
EXIT = re.compile(TS + r".*(state exiting \(clip end or cancel\)|notified SH2_CastExit)")


def stamp(raw: str) -> datetime:
    return datetime.strptime(raw, "%Y-%m-%d %H:%M:%S.%f")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--window-ms", type=float, default=60.0)
    ap.add_argument("--since", default=None, help='"YYYY-MM-DD HH:MM:SS" -- ignore earlier lines')
    args = ap.parse_args()

    since = datetime.strptime(args.since, "%Y-%m-%d %H:%M:%S") if args.since else None

    accepts: list[tuple[datetime, str, int]] = []
    isolates: list[tuple[datetime, str]] = []
    entries: list[tuple[datetime, str]] = []
    exits: list[datetime] = []

    with open(args.log, encoding="utf-8", errors="replace") as fh:
        for lineno, line in enumerate(fh, 1):
            for pattern, sink in (
                (ACCEPT, lambda t, m: accepts.append((t, m.group(2), lineno))),
                (ISOLATE, lambda t, m: isolates.append((t, m.group(2)))),
                (ENTRY, lambda t, m: entries.append((t, m.group(2)))),
                (EXIT, lambda t, _m: exits.append(t)),
            ):
                m = pattern.search(line)
                if not m:
                    continue
                t = stamp(m.group(1))
                if since and t < since:
                    continue
                sink(t, m)

    orphans = []
    for when, hand, lineno in accepts:
        paired = any(
            ih == hand and 0 <= (when - it).total_seconds() * 1000.0 <= args.window_ms
            for it, ih in isolates
        )
        if not paired:
            last_entry = max((t for t, _ in entries if t <= when), default=None)
            last_exit = max((t for t in exits if t <= when), default=None)
            live = last_entry is not None and (last_exit is None or last_exit < last_entry)
            orphans.append((when, hand, lineno, live))

    print(f"accepted SpellFire events : {len(accepts)}")
    print(f"isolation lines           : {len(isolates)}")
    print(f"cast entries / exits      : {len(entries)} / {len(exits)}")
    print(f"orphans (accepted, not isolated): {len(orphans)}")
    for when, hand, lineno, live in orphans:
        state = "driver state LIVE" if live else "no driver state"
        print(f"  line {lineno}: {when} {hand} -- {state}")

    return 1 if orphans else 0


if __name__ == "__main__":
    sys.exit(main())
