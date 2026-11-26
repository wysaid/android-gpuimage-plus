#!/usr/bin/env python3
"""Add a Gradle publication to the Maven repository without replacing releases."""

import argparse
import hashlib
import shutil
import xml.etree.ElementTree as ET
from pathlib import Path


def sync_release(source: Path, target: Path) -> None:
    metadata_name = "maven-metadata.xml"
    metadata = ET.parse(source / metadata_name)
    artifact_id = metadata.findtext("artifactId")
    incoming_versions = metadata.findall("versioning/versions/version")
    if not artifact_id or not incoming_versions:
        raise ValueError("The staged Maven metadata has no artifact ID or versions")

    new_versions = []
    for entry in incoming_versions:
        version = entry.text
        if not version:
            raise ValueError("The staged Maven metadata contains an empty version")

        source_version = source / version
        target_version = target / version
        already_published = target_version.exists()
        filenames = [f"{artifact_id}-{version}.{ext}" for ext in ("aar", "pom")]
        for filename in filenames:
            staged_file = source_version / filename
            if not staged_file.is_file():
                raise ValueError(f"Missing staged artifact: {staged_file}")
            if already_published:
                published_file = target_version / filename
                if (
                    not published_file.is_file()
                    or published_file.read_bytes() != staged_file.read_bytes()
                ):
                    raise ValueError(
                        f"Maven version {version} is already published with different artifacts"
                    )
        if not already_published:
            new_versions.append(version)

    if not new_versions:
        return

    target_metadata = target / metadata_name
    versions = metadata.find("versioning/versions")
    if target_metadata.is_file():
        published = ET.parse(target_metadata).findall("versioning/versions/version")
        all_versions = dict.fromkeys(entry.text for entry in published + incoming_versions)
        versions.clear()
        for version in all_versions:
            ET.SubElement(versions, "version").text = version

    target.mkdir(parents=True, exist_ok=True)
    for version in new_versions:
        shutil.copytree(source / version, target / version)

    ET.indent(metadata, space="  ")
    payload = ET.tostring(metadata.getroot(), encoding="UTF-8", xml_declaration=True) + b"\n"
    target_metadata.write_bytes(payload)
    for algorithm in ("md5", "sha1", "sha256", "sha512"):
        checksum = target / f"{metadata_name}.{algorithm}"
        checksum.write_text(hashlib.new(algorithm, payload).hexdigest(), encoding="ascii")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="Gradle publication directory for this artifact")
    parser.add_argument("target", type=Path, help="Published Maven directory for this artifact")
    args = parser.parse_args()
    try:
        sync_release(args.source, args.target)
    except (OSError, ET.ParseError, ValueError) as error:
        parser.exit(1, f"{error}\n")
