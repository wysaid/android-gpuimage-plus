#!/usr/bin/env python3
"""Validate page alignment and GNU RELRO coverage in native AAR libraries."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
import tempfile
import zipfile
from dataclasses import dataclass
from pathlib import Path

PAGE_SIZE_4K = 4096
PAGE_SIZE_16K = 16384

PROGRAM_HEADER_RE = re.compile(
    r"^\s*(LOAD|GNU_RELRO)\s+"
    r"(0x[0-9a-f]+)\s+"  # Offset
    r"(0x[0-9a-f]+)\s+"  # VirtAddr
    r"(0x[0-9a-f]+)\s+"  # PhysAddr
    r"(0x[0-9a-f]+)\s+"  # FileSiz
    r"(0x[0-9a-f]+)\s+"  # MemSiz
    r".*?\s+(0x[0-9a-f]+)\s*$",  # Align
    re.IGNORECASE,
)


@dataclass(frozen=True)
class Segment:
    kind: str
    virtual_address: int
    memory_size: int
    alignment: int

    @property
    def end(self) -> int:
        return self.virtual_address + self.memory_size


def align_down(value: int, alignment: int) -> int:
    return value // alignment * alignment


def align_up(value: int, alignment: int) -> int:
    return (value + alignment - 1) // alignment * alignment


def read_program_headers(shared_library: Path) -> list[Segment]:
    result = subprocess.run(
        ["readelf", "-lW", str(shared_library)],
        check=False,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "readelf failed")

    segments: list[Segment] = []
    for line in result.stdout.splitlines():
        match = PROGRAM_HEADER_RE.match(line)
        if match is None:
            continue
        segments.append(
            Segment(
                kind=match.group(1).upper(),
                virtual_address=int(match.group(3), 16),
                memory_size=int(match.group(6), 16),
                alignment=int(match.group(7), 16),
            )
        )
    return segments


def validate_shared_library(
    aar: Path, entry_name: str, shared_library: Path, require_16k: bool
) -> list[str]:
    try:
        segments = read_program_headers(shared_library)
    except RuntimeError as error:
        return [f"{aar.name}:{entry_name}: {error}"]

    load_segments = [segment for segment in segments if segment.kind == "LOAD"]
    relro_segments = [
        segment for segment in segments if segment.kind == "GNU_RELRO"
    ]
    errors: list[str] = []

    if not load_segments:
        return [f"{aar.name}:{entry_name}: no PT_LOAD segments found"]

    if require_16k:
        for load in load_segments:
            if load.alignment < PAGE_SIZE_16K:
                errors.append(
                    f"{aar.name}:{entry_name}: PT_LOAD alignment "
                    f"{load.alignment} is less than {PAGE_SIZE_16K}"
                )

    for relro in relro_segments:
        relro_page_start = align_down(relro.virtual_address, PAGE_SIZE_4K)
        relro_page_end = align_up(relro.end, PAGE_SIZE_4K)
        covering_load = next(
            (
                load
                for load in load_segments
                if align_down(load.virtual_address, PAGE_SIZE_4K)
                <= relro_page_start
                and align_up(load.end, PAGE_SIZE_4K) >= relro_page_end
            ),
            None,
        )
        if covering_load is None:
            errors.append(
                f"{aar.name}:{entry_name}: 4KB-rounded PT_GNU_RELRO range "
                f"{relro_page_start:#x}-{relro_page_end:#x} exceeds its "
                "mapped PT_LOAD pages"
            )

    return errors


def validate_aar(aar: Path) -> list[str]:
    require_16k = "-16k" in aar.stem
    errors: list[str] = []

    if not aar.is_file():
        return [f"{aar}: file not found"]

    with zipfile.ZipFile(aar) as archive, tempfile.TemporaryDirectory() as temp_dir:
        native_entries = sorted(
            (
                info
                for info in archive.infolist()
                if info.filename.startswith("jni/")
                and info.filename.endswith(".so")
            ),
            key=lambda info: info.filename,
        )
        if not native_entries:
            return [f"{aar.name}: no native libraries found"]

        extraction_root = Path(temp_dir)
        for entry in native_entries:
            shared_library = Path(archive.extract(entry, extraction_root))
            errors.extend(
                validate_shared_library(
                    aar, entry.filename, shared_library, require_16k
                )
            )

    if not errors:
        alignment = (
            "16KB LOAD alignment and 4KB RELRO coverage"
            if require_16k
            else "4KB RELRO coverage"
        )
        print(f"✓ {aar.name}: {alignment} valid")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate native ELF layouts in published AAR files."
    )
    parser.add_argument("aars", nargs="+", type=Path)
    args = parser.parse_args()

    errors: list[str] = []
    for aar in args.aars:
        errors.extend(validate_aar(aar))

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
