#!/usr/bin/env python3
"""Capture and summarize a "root signature" -- what correct casting commitment looks
like in telemetry.

Two channels, because no single DevBench surface carries both halves:

  pose   `record` samples x/y/z/angleZ/angleX + game frame on a BACKGROUND thread at
         `--pose-interval-ms`, so player translation and heading are sampled at a
         cadence the main-thread task queue could never hold.
  vars   a foreground poll of the animation graph (`bAnimationDriven`,
         `IsCastingRight`, `IsCastingLeft`) plus the game frame, one `scenario` per
         tick -- roughly 160 ms measured. The frame stamp is what aligns this series
         to the pose trajectory.

Clip activations are paged out of `cliplog`'s 512-entry ring every tick with `since`,
so a long capture next to a busy actor loses nothing.

Usage:
  python tools/telemetry/root_signature.py capture --label msco-ff-moving --seconds 20 \
      --out .scratch/mco-integration/evidence/t54
  python tools/telemetry/root_signature.py summarize <capture.json> --write

The player must be driven by hand: injected movement keys do not move the player
(docs/agents/headless-testing-playbook.md), so every moving-entry capture is
owner-hands by construction. This script only records.
"""

import argparse
import json
import math
import os
import shutil
import sys
import time
import urllib.error
import urllib.request

BASE = "http://127.0.0.1:8920/api/tool"
VARS = ["bAnimationDriven", "IsCastingRight", "IsCastingLeft"]
OVERWRITE = r"C:\Nolvus\Instances\Nolvus Awakening\MODS\overwrite"


def call(tool, args, timeout=25):
    body = json.dumps(args).encode()
    req = urllib.request.Request(
        BASE + "/" + tool, data=body, headers={"content-type": "application/json"}
    )
    try:
        with urllib.request.urlopen(req, timeout=timeout) as fh:
            return json.loads(fh.read().decode())
    except urllib.error.HTTPError as exc:
        return {"error": exc.read().decode()[:300], "code": exc.code}
    except Exception as exc:  # noqa: BLE001 -- surfaced in the transcript, not swallowed
        return {"error": repr(exc)}


def anim_step(var):
    return {
        "tool": "papyrus",
        "args": {
            "action": "call",
            "script": "Actor",
            "function": "GetAnimationVariableBool",
            "self": {"form": "0x14"},
            "args": [var],
        },
    }


TICK_STEPS = [{"tool": "inspect", "args": {"kind": "state"}}] + [anim_step(v) for v in VARS]


def tick():
    """One variable sample, stamped with the game frame it started on."""
    res = call("scenario", {"steps": TICK_STEPS, "continueOnError": True})
    out = {"wall": time.time(), "ok": res.get("ok")}
    results = res.get("results") or []
    if len(results) != len(TICK_STEPS):
        out["error"] = res.get("error") or "short transcript"
        return out
    out["frame"] = (results[0].get("result") or {}).get("frame")
    for var, entry in zip(VARS, results[1:]):
        r = entry.get("result") or {}
        out[var] = r.get("returned") if entry.get("ok") else None
    return out


def resolve_vfs(path):
    """MO2 redirects the plugin's writes into the instance overwrite tree; the tool
    reports the path relative to the virtual Data directory."""
    p = path.replace("\\", "/")
    parts = p.split("/Data/", 1)
    rel = parts[1] if len(parts) == 2 else (p[5:] if p.startswith("Data/") else None)
    if not rel:
        return None
    return os.path.join(OVERWRITE, rel.replace("/", os.sep))


def preflight():
    state = call("inspect", {"kind": "state"})
    if not state.get("playerLoaded"):
        sys.exit("not ready for capture: " + json.dumps(state)[:300])
    return state


def capture(args):
    os.makedirs(args.out, exist_ok=True)
    state = preflight()
    scene = call("inspect", {"kind": "scene"})

    call("cliplog", {"action": "clear"})
    clip_start = call("cliplog", {"action": "start"})
    rec_wall = time.time()
    rec_start = call("record", {"action": "start", "intervalMs": args.pose_interval_ms})

    samples, clips = [], []
    last_seq = 0
    t0 = time.time()
    print("CAPTURING %s (%s) for %ss -- perform the run NOW" % (args.label, args.mode, args.seconds), flush=True)
    while time.time() - t0 < args.seconds:
        if args.mode == "full":
            samples.append(tick())
        else:
            time.sleep(0.25)
        page = call("cliplog", {"action": "read", "since": last_seq, "limit": 200})
        for entry in page.get("entries") or []:
            clips.append(entry)
            last_seq = max(last_seq, entry.get("seq", last_seq))
    print("capture window closed", flush=True)

    rec_stop = call("record", {"action": "stop"})
    call("cliplog", {"action": "stop"})
    tail = call("cliplog", {"action": "read", "since": last_seq, "limit": 500})
    clips.extend(tail.get("entries") or [])

    pose_path, pose = None, None
    src = (rec_stop or {}).get("path")
    if src:
        dest = os.path.join(args.out, args.label + "-pose.json")
        for candidate in (src, resolve_vfs(src)):
            if candidate and os.path.exists(candidate):
                shutil.copyfile(candidate, dest)
                pose_path = dest
                break
        if pose_path:
            with open(pose_path, encoding="utf-8") as fh:
                pose = json.load(fh)

    doc = {
        "label": args.label,
        "note": args.note,
        "mode": args.mode,
        "capturedAt": time.strftime("%Y-%m-%dT%H:%M:%S%z"),
        "recordStartWall": rec_wall,
        "seconds": args.seconds,
        "poseIntervalMs": args.pose_interval_ms,
        "state": state,
        "scene": scene,
        "clipProbe": clip_start,
        "recordStart": rec_start,
        "recordStop": rec_stop,
        "poseFile": pose_path,
        "poseSampleCount": len(pose_samples(pose)) if pose else None,
        "varSamples": samples,
        "clips": clips,
    }
    out_path = os.path.join(args.out, args.label + ".json")
    with open(out_path, "w", encoding="utf-8") as fh:
        json.dump(doc, fh, indent=1)
    print("wrote %s (%d var samples, %d clip activations)" % (out_path, len(samples), len(clips)))
    if pose_path:
        print("wrote %s (%s pose samples)" % (pose_path, doc["poseSampleCount"]))
    else:
        print("POSE NOT COPIED -- recording lives at " + str(src), file=sys.stderr)
    return out_path


def pose_samples(pose):
    """Flatten a devbench-recording-2 into [{tMs, x, y, z, angleZ, angleX, held}].

    The recorder DEDUPES: a sample identical to the previous one is written as a bare
    {"wait": ms} step with no pose. Those are forward-filled here and flagged
    held=True, which is exactly the signal a root produces -- an unchanging pose.
    """
    if not pose:
        return []
    out = []
    t = 0.0
    last = None
    for step in pose.get("steps") or []:
        if "pose" not in step:
            if "wait" in step and last is not None:
                out.append(dict(last, tMs=t, held=True))
                t += step["wait"]
            continue
        x, y, z, angle_z, angle_x = (list(step["pose"]) + [None] * 5)[:5]
        last = {"x": x, "y": y, "z": z, "angleZ": angle_z, "angleX": angle_x}
        out.append(dict(last, tMs=t, held=False))
        t += step.get("wait", 0)
    return out


def summarize(args):
    with open(args.capture, encoding="utf-8") as fh:
        doc = json.load(fh)
    pose = None
    if doc.get("poseFile") and os.path.exists(doc["poseFile"]):
        with open(doc["poseFile"], encoding="utf-8") as fh:
            pose = json.load(fh)

    lines = ["# Root signature: " + doc["label"], ""]
    if doc.get("note"):
        lines += ["Owner's description: " + doc["note"], ""]

    # Both channels are put on one clock: milliseconds since `record` started.
    rec_wall = doc.get("recordStartWall")
    samples = [s for s in doc.get("varSamples") or [] if s.get("frame")]
    for s in samples:
        s["tMs"] = (s["wall"] - rec_wall) * 1000.0 if rec_wall else None

    edges = []
    prev = None
    for s in samples:
        cur = dict((v, s.get(v)) for v in VARS)
        if prev is not None:
            for v in VARS:
                if cur[v] != prev[v]:
                    edges.append((s["tMs"], s["frame"], v, prev[v], cur[v]))
        prev = cur

    if samples:
        gaps = [b["tMs"] - a["tMs"] for a, b in zip(samples, samples[1:])]
        lines.append(
            "Variable channel: %d samples, median cadence %.0f ms."
            % (len(samples), sorted(gaps)[len(gaps) // 2] if gaps else 0)
        )
        lines.append("")
        lines.append("## Variable edges (t ms, frame, var, from -> to)")
        lines += ["- %7.0f  f%s  %s  %s -> %s" % e for e in edges] or ["- none observed"]
        lines.append("")

    pts = pose_samples(pose)
    if pts:
        gaps = [b["tMs"] - a["tMs"] for a, b in zip(pts, pts[1:])]
        lines.append(
            "## Pose: %d samples, median cadence %.0f ms (requested %s ms)"
            % (len(pts), sorted(gaps)[len(gaps) // 2] if gaps else 0, doc["poseIntervalMs"])
        )
        steps = []
        for a, b in zip(pts, pts[1:]):
            if None in (a["x"], a["y"], b["x"], b["y"]):
                continue
            dt = max(b["tMs"] - a["tMs"], 1.0)
            steps.append(
                {
                    "tMs": b["tMs"],
                    "d": math.dist((a["x"], a["y"]), (b["x"], b["y"])),
                    "speed": math.dist((a["x"], a["y"]), (b["x"], b["y"])) / dt * 1000.0,
                    "angleZ": b["angleZ"],
                }
            )
        peak = max((s["speed"] for s in steps), default=0.0)
        lines.append("Peak speed in window: %.0f units/s." % peak)

        cast_t = next((t for t, _f, _v, _a, b in edges if b is True), None)
        if cast_t is not None and steps:
            lines.append("Cast begin at t=%.0f ms (first variable rise)." % cast_t)
            after = [s for s in steps if s["tMs"] >= cast_t]
            settle = next((s for s in after if s["speed"] < args.settle_speed), None)
            if settle:
                lines.append(
                    "Speed < %.0f units/s at t=%.0f ms -- %.0f ms after cast begin."
                    % (args.settle_speed, settle["tMs"], settle["tMs"] - cast_t)
                )
                tail = sum(s["d"] for s in after if s["tMs"] <= settle["tMs"])
                lines.append("Momentum tail: %.1f units travelled over that span." % tail)
            else:
                lines.append(
                    "NEVER settled below %.0f units/s inside the window." % args.settle_speed
                )
            heads = [s["angleZ"] for s in after if s["angleZ"] is not None]
            if heads:
                lines.append(
                    "Heading (angleZ) after cast begin: %.1f..%.1f deg (span %.1f)"
                    % (min(heads), max(heads), max(heads) - min(heads))
                )
        else:
            lines.append(
                "No cast-begin edge in the variable series -- pose reported unanchored."
            )
        lines.append("")

    lines.append("## Clip activations")
    seen = {}
    for c in doc["clips"]:
        key = (c.get("clip"), c.get("mod"), c.get("submod"))
        seen[key] = seen.get(key, 0) + 1
    ranked = sorted(seen.items(), key=lambda kv: -kv[1])[: args.top]
    lines += [
        "- %3dx %s  [%s / %s]" % (n, clip, mod or "-", submod or "-")
        for (clip, mod, submod), n in ranked
    ] or ["- none recorded"]
    lines.append("")

    text = "\n".join(lines)
    print(text)
    if args.write:
        out = os.path.splitext(args.capture)[0] + "-summary.md"
        with open(out, "w", encoding="utf-8") as fh:
            fh.write(text + "\n")
        print("wrote " + out, file=sys.stderr)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    sub = ap.add_subparsers(dest="cmd", required=True)

    c = sub.add_parser("capture")
    c.add_argument("--label", required=True)
    c.add_argument("--seconds", type=float, default=20.0)
    c.add_argument("--pose-interval-ms", type=int, default=50)
    c.add_argument("--out", default=".scratch/mco-integration/evidence/t54")
    c.add_argument("--note", default="")
    c.add_argument(
        "--mode",
        choices=["full", "pose"],
        default="full",
        help="full = variables + pose + clips (pose cadence drops to ~300 ms, the "
        "main-thread polls starve the sampler); pose = pose + clips only, ~65 ms",
    )
    c.set_defaults(func=capture)

    s = sub.add_parser("summarize")
    s.add_argument("capture")
    s.add_argument("--top", type=int, default=15)
    s.add_argument("--settle-speed", type=float, default=20.0, help="units/s counted as rooted")
    s.add_argument("--write", action="store_true")
    s.set_defaults(func=summarize)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
