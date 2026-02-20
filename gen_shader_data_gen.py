#!/usr/bin/env python3
"""Generate shader_data_gen.inc from Cg shader sources.

Scans a shader directory for matching pairs:
  - **/*.vs.cg
  - **/*.ps.cg

Additionally warns if it finds any regular files that don't match those patterns
(e.g. stray backups, wrong extensions), since those "might not belong here".

Assumptions (matches common Makefile setups):
- Shader sources live under: src/shaders/**/{name}.vs.cg and {name}.ps.cg
- The compiled/converted include files will be available under the build output
  dir using the SAME relative path, e.g.:
    src_out/shaders/**/{name}.vs.inl
    src_out/shaders/**/{name}.ps.inl
- This script only generates the *aggregator* include file (shader_data_gen.inc)
  that #includes the per-shader .inl files.

Build-friendly behavior:
- Writes to a temporary file and only replaces the output if the contents changed,
  so you can run it every build without needlessly forcing recompiles.

Usage:
  python3 gen_shader_data_gen.py \
      --shader-dir src/shaders \
      --out src_out/shader_data_gen.inc \
      --inl-prefix shaders

Defaults match the above.

Exit codes:
  0 success
  2 missing shader pairs and/or unexpected files (warnings shown; still generates for complete pairs)
  1 fatal error (e.g. shader dir missing)
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

_ALLOWED_SUFFIXES = (".vs.cg", ".ps.cg")


def _c_ident(s: str) -> str:
    # Create a safe C identifier from a path-like stem.
    ident = re.sub(r"[^0-9A-Za-z_]", "_", s)
    ident = re.sub(r"_+", "_", ident).strip("_")
    if not ident:
        ident = "shader"
    if ident[0].isdigit():
        ident = "s_" + ident
    return ident


def _is_allowed_shader_file(p: Path) -> bool:
    name = p.name
    return name.endswith(_ALLOWED_SUFFIXES)


def find_unexpected_files(shader_dir: Path) -> list[Path]:
    unexpected: list[Path] = []
    for p in shader_dir.rglob("*"):
        if not p.is_file():
            continue
        if _is_allowed_shader_file(p):
            continue
        unexpected.append(p)
    return sorted(unexpected)


def discover_stems(shader_dir: Path) -> list[str]:
    # Find all .vs.cg, require matching .ps.cg for generation.
    stems: set[str] = set()
    for vs in shader_dir.rglob("*.vs.cg"):
        rel = vs.relative_to(shader_dir).as_posix()
        stem = rel[: -len(".vs.cg")]  # drop suffix
        stems.add(stem)
    return sorted(stems)


def generate(shader_dir: Path, out_path: Path, inl_prefix: str) -> int:
    stems = discover_stems(shader_dir)

    unexpected = find_unexpected_files(shader_dir)
    missing_pairs: list[str] = []

    blocks: list[str] = []
    blocks.append("/* Auto-generated from shader .cg sources - do not edit */\n")

    for stem in stems:
        ps = shader_dir / f"{stem}.ps.cg"
        if not ps.exists():
            missing_pairs.append(str(ps))
            continue

        ident = _c_ident(stem)

        # Include paths relative to the output file's directory (usually src_out/)
        vs_inl = f"{inl_prefix}/{stem}.vs.inl" if inl_prefix else f"{stem}.vs.inl"
        ps_inl = f"{inl_prefix}/{stem}.ps.inl" if inl_prefix else f"{stem}.ps.inl"

        blocks.append("\n")
        blocks.append(f"static const u32 g_vs_program_{ident}[] = {{\n")
        blocks.append(f"#include \"{vs_inl}\"\n")
        blocks.append("};\n\n")
        blocks.append(f"static void setup_fragment_shader_{ident}(u32 **pp) {{\n")
        blocks.append("    u32 *p = *pp;\n")
        blocks.append(f"#include \"{ps_inl}\"\n")
        blocks.append("    *pp = p;\n")
        blocks.append("}\n")

    out_text = "".join(blocks)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    tmp = out_path.with_suffix(out_path.suffix + ".tmp")
    tmp.write_text(out_text, encoding="utf-8")

    # Only update if changed
    changed = True
    if out_path.exists():
        try:
            changed = out_path.read_text(encoding="utf-8") != out_text
        except UnicodeDecodeError:
            changed = True

    if changed:
        tmp.replace(out_path)
        print(f"[shader_data_gen] wrote: {out_path}")
    else:
        tmp.unlink(missing_ok=True)
        print(f"[shader_data_gen] up-to-date: {out_path}")

    warned = False

    if missing_pairs:
        warned = True
        print("[shader_data_gen] WARNING: missing matching .ps.cg for:", file=sys.stderr)
        for m in missing_pairs:
            print(f"  - {m}", file=sys.stderr)

    if unexpected:
        warned = True
        print("[shader_data_gen] WARNING: unexpected files found in shader dir (might not belong here):", file=sys.stderr)
        for p in unexpected:
            rel = p.relative_to(shader_dir)
            print(f"  - {rel.as_posix()}", file=sys.stderr)
        print("[shader_data_gen] Only *.vs.cg and *.ps.cg are expected under the shader directory.", file=sys.stderr)

    return 2 if warned else 0


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--shader-dir", default="src/shaders", help="Directory containing *.vs.cg/*.ps.cg")
    ap.add_argument("--out", default="src_out/shader_data_gen.inc", help="Output .inc file")
    ap.add_argument(
        "--inl-prefix",
        default="shaders",
        help="Prefix used in #include paths (relative to output dir). Use '' for none.",
    )
    args = ap.parse_args(argv)

    shader_dir = Path(args.shader_dir).resolve()
    out_path = Path(args.out).resolve()
    inl_prefix = args.inl_prefix.strip("/")

    if not shader_dir.exists():
        print(f"shader dir not found: {shader_dir}", file=sys.stderr)
        return 1

    return generate(shader_dir=shader_dir, out_path=out_path, inl_prefix=inl_prefix)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
