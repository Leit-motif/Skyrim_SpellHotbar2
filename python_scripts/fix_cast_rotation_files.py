"""Align the cast pack's file positions with MSCO's own rotation (ticket 39 follow-on).

SH2's CastComboIndex walks pack positions 1-2-3-4, but MSCO's rotation is authored as
@SGVI successor payloads in the clips themselves. Derived 2026-08-29 from the live
sources (see ticket 39):

  aimed left  (Base - default    MSCO_left):  1 -> 5 -> 3 -> 4   (5 = stationary left2)
  aimed right (Base - default    MSCO_right): 1 -> 5 -> 3 -> 4   (5 = stationary right2)
  self left/right (Base - Self *):            1 -> 2 -> 1 -> 2   (source slots 1,3,4 identical)
  staff left/right, dual:                     1 -> 2 -> 3 -> 4   (already what the pack ships)

So two kinds of position fix, each a source copy plus the family's stamp annotation:
  cast_left/MSCO_left2  <- Base - default  MSCO_left5    stamp SH2_PackStamp_cast_left
  cast_right/MSCO_left2 <- Base - default  MSCO_right5   stamp SH2_PackStamp_cast_right
  cast_left_self/MSCO_left4  <- Base - Self Left  MSCO_left2   stamp SH2C_cast_left_self_4
  cast_right_self/MSCO_left4 <- Base - Self Right MSCO_right2  stamp SH2C_cast_right_self_4

Stamps follow ticket 60's conventions (submod-wide name for aimed, per-file for self) and
sit at the source clip's duration. Requires hkxc-anno-cli and hkxc (see the
skyrim-hkx-annotations skill's tools-config).
"""

import re
import shutil
import subprocess
import sys
from pathlib import Path

CLI = Path(r"C:\Tools\SkyrimHKX\hkxc-anno-cli.exe")
HKXC = Path(r"C:\Tools\SkyrimHKX\hkxc.exe")
MSCO = Path(
    r"C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\MSCO Magic Casting Behavior Overhaul"
    r"\meshes\actors\character\animations\OpenAnimationReplacer\MSCO Animations"
)
PACK = Path(__file__).resolve().parents[1] / (
    "data/meshes/actors/character/animations/OpenAnimationReplacer/SpellHotbar2Casts"
)

SWAPS = [
    ("Base - default/MSCO_left5.hkx", "cast_left/MSCO_left2.hkx", "SH2_PackStamp_cast_left"),
    ("Base - default/MSCO_right5.hkx", "cast_right/MSCO_left2.hkx", "SH2_PackStamp_cast_right"),
    ("Base - Self Left/MSCO_left2.hkx", "cast_left_self/MSCO_left4.hkx", "SH2C_cast_left_self_4"),
    ("Base - Self Right/MSCO_right2.hkx", "cast_right_self/MSCO_left4.hkx", "SH2C_cast_right_self_4"),
]


def run(*args: object) -> None:
    subprocess.run([str(a) for a in args], check=True, capture_output=True)


def main() -> int:
    scratch = PACK.parent / "_rotation_fix_tmp"
    scratch.mkdir(exist_ok=True)
    for src_rel, dst_rel, stamp in SWAPS:
        src = MSCO / src_rel
        dst = PACK / dst_rel
        work = scratch / dst.name
        shutil.copyfile(src, work)
        txt = work.with_suffix(".txt")
        run(CLI, "dump", "-i", work, "-o", txt)
        body = txt.read_text(encoding="utf-8")
        duration = float(re.search(r"# duration: ([0-9.]+)", body).group(1))
        count = int(re.search(r"# annotations: (\d+)", body).group(1))
        body = body.replace(f"# annotations: {count}", f"# annotations: {count + 1}", 1)
        if not body.endswith("\n"):
            body += "\n"
        body += f"{duration:.6f} {stamp}\n"
        txt.write_text(body, encoding="utf-8")
        run(CLI, "update", "-a", txt, "-i", work)
        run(HKXC, "verify", work)
        shutil.copyfile(work, dst)
        print(f"{dst_rel} <- {src_rel} (+{stamp} @ {duration:.6f})")
    shutil.rmtree(scratch)
    print("done")
    return 0


if __name__ == "__main__":
    sys.exit(main())
