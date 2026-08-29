"""Report every tracker ticket's effective status, and flag the files a header scan misreads.

The tracker (`.scratch/<effort>/issues/NN-<slug>.md`) records triage state in a `Status:` line,
but three formats are in use — `**Status:** open`, `**Status: CLOSED ...**`, and a `**Type:** x.
**Status:** y` pair on one line — and a ticket that closes late often gains a SECOND status line
lower down while the header keeps the stale one. The 2026-08-29 sweep found four such files. This
script is the check that stops the fifth.

Usage:
    python python_scripts/ticket_status.py            # table of every ticket
    python python_scripts/ticket_status.py --open     # only what is actually on the frontier
    python python_scripts/ticket_status.py --lint     # exit 1 if any file is ambiguous
"""

from __future__ import annotations

import argparse
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SCRATCH = REPO / ".scratch"

# Matches all three formats; captures whatever follows the word Status.
STATUS_RE = re.compile(r"^\*{0,2}Status:?\*{0,2}\s*:?\s*(.+?)\s*$", re.M)
INLINE_RE = re.compile(r"\*\*Status:?\*{0,2}\s*(.+?)(?:\*\*)?\s*$", re.M)
# A sweep marks a stale or historical declaration by putting the reason in the bold label
# itself, e.g. `**Status (superseded -- see the top):**`. Those are records of what a ticket
# once said, not live declarations, so they do not count toward its status.
STRUCK_WORDS = ("supersed", "historical", "was ", "former", "at the time")


def is_struck(line: str) -> bool:
    if not line.startswith("**Status"):
        return False
    label = line[len("**Status"):].split(":", 1)[0].lower()
    return any(word in label for word in STRUCK_WORDS)

# A status counts as closed if its text opens with one of these.
CLOSED_WORDS = (
    "closed", "resolved", "done", "superseded", "wontfix", "deferred",
    "dropped", "parked", "spec",
)
OPEN_WORDS = ("ready-for-agent", "needs-triage", "needs-info", "claimed", "open", "in progress")


def classify(text: str) -> str:
    # A status often keeps its own history inline as strikethrough, e.g.
    # `~~claimed~~ **superseded 2026-08-12**`. Only the unstruck remainder is live.
    unstruck = re.sub(r"~~.*?~~", "", text)
    low = (unstruck if unstruck.strip(" *_") else text).lower().lstrip("~*_ ")
    for word in CLOSED_WORDS:
        if low.startswith(word):
            return "closed"
    for word in OPEN_WORDS:
        if low.startswith(word):
            return "open"
    return "unknown"


def status_lines(body: str) -> list[tuple[int, str]]:
    """Every status declaration in the file, as (line number, status text).

    A status routinely wraps over several lines and its verdict can land on the second one
    (`~~claimed~~ ~~open -- released~~` / `**resolved by audit**`), so each declaration is read
    as the whole paragraph, up to the next blank line.
    """
    lines = body.splitlines()
    found = []
    for num, line in enumerate(lines, start=1):
        if "Status" not in line:
            continue
        if is_struck(line):
            continue  # deliberately struck by a sweep; not a live declaration
        match = INLINE_RE.search(line) or STATUS_RE.match(line)
        if not match:
            continue
        text = [match.group(1)]
        for follow in lines[num:]:
            if not follow.strip():
                break
            text.append(follow.strip())
        found.append((num, " ".join(text)))
    return found


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--open", action="store_true", help="only tickets still on the frontier")
    parser.add_argument("--lint", action="store_true", help="exit 1 on any ambiguous file")
    args = parser.parse_args()

    problems: list[str] = []
    rows: list[tuple[str, str, str]] = []

    for path in sorted(SCRATCH.glob("*/issues/*.md")):
        rel = path.relative_to(SCRATCH).as_posix()
        body = path.read_text(encoding="utf-8", errors="replace")
        found = status_lines(body)

        if not found:
            problems.append(f"{rel}: no Status line at all")
            rows.append((rel, "?", "MISSING"))
            continue

        kinds = {classify(text) for _, text in found}
        first_line, first_text = found[0]

        if "unknown" in kinds:
            unknown = [f"L{n}: {t[:50]}" for n, t in found if classify(t) == "unknown"]
            problems.append(f"{rel}: unrecognised status ({'; '.join(unknown)})")
        if kinds == {"open", "closed"} or ("closed" in kinds and "open" in kinds):
            where = ", ".join(f"L{n} {classify(t)}" for n, t in found)
            problems.append(f"{rel}: status lines disagree ({where}) -- a header scan may misread it")

        # A file is closed if ANY line says so; a late close is still a close.
        state = "closed" if "closed" in kinds else classify(first_text)
        rows.append((rel, state, first_text[:72]))

    if args.lint:
        for line in problems:
            print("AMBIGUOUS  " + line)
        print(f"\n{len(problems)} ambiguous file(s) of {len(rows)} tickets.")
        return 1 if problems else 0

    for rel, state, text in rows:
        if args.open and state != "open":
            continue
        print(f"{state:<8} {rel:<56} {text}")

    if not args.open:
        print(f"\n{sum(1 for _, s, _ in rows if s == 'open')} open of {len(rows)} tickets.")
        if problems:
            print(f"{len(problems)} ambiguous — run with --lint for detail.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
