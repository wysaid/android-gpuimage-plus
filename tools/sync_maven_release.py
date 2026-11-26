#!/usr/bin/env python3
"""Add a Gradle publication to the Maven repository without replacing releases."""

import argparse
import hashlib
import shutil
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path


def sync_release(source: Path, target: Path, release_version: str, source_commit: str) -> None:
    metadata_name = "maven-metadata.xml"
    metadata = ET.parse(source / metadata_name)
    artifact_id = metadata.findtext("artifactId")
    versioning = metadata.find("versioning")
    versions = metadata.find("versioning/versions")
    if not artifact_id or versioning is None or versions is None:
        raise ValueError("The staged Maven metadata has no artifact ID or version list")

    incoming_versions = [entry.text for entry in versions.findall("version")]
    expected_versions = {release_version + suffix for suffix in ("", "-min", "-16k", "-16k-min")}
    if len(incoming_versions) != len(expected_versions) or set(incoming_versions) != expected_versions:
        raise ValueError(
            f"Staged Maven versions do not match {release_version}: {incoming_versions}"
        )

    source_commit_file = target / release_version / "source-commit.txt"
    reuse_published = source_commit_file.is_file()
    if reuse_published and source_commit_file.read_text(encoding="ascii").strip() != source_commit:
        raise ValueError(f"Maven release {release_version} was published from a different commit")

    new_versions = []
    for version in incoming_versions:
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
                    or (not reuse_published and published_file.read_bytes() != staged_file.read_bytes())
                ):
                    raise ValueError(
                        f"Maven version {version} is already published with different artifacts"
                    )
        if not already_published:
            new_versions.append(version)

    if not new_versions:
        return
    if reuse_published:
        raise ValueError(
            f"Maven release {release_version} is missing published variants: {new_versions}"
        )

    target_metadata = target / metadata_name
    updated_at = datetime.now(timezone.utc).strftime("%Y%m%d%H%M%S")
    if target_metadata.is_file():
        published = ET.parse(target_metadata)
        published_versions = published.findall("versioning/versions/version")
        all_versions = dict.fromkeys([entry.text for entry in published_versions] + incoming_versions)
        versions.clear()
        for version in all_versions:
            ET.SubElement(versions, "version").text = version
        updated_at = max(updated_at, published.findtext("versioning/lastUpdated", default=""))

    last_updated = versioning.find("lastUpdated")
    if last_updated is None:
        last_updated = ET.SubElement(versioning, "lastUpdated")
    last_updated.text = updated_at

    target.mkdir(parents=True, exist_ok=True)
    for version in new_versions:
        shutil.copytree(source / version, target / version)
    source_commit_file.write_text(source_commit + "\n", encoding="ascii")

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
    parser.add_argument("release_version", help="Expected Maven version from the release tag")
    parser.add_argument("source_commit", help="Source commit used to build the release tag")
    args = parser.parse_args()
    try:
        sync_release(args.source, args.target, args.release_version, args.source_commit)
    except (OSError, ET.ParseError, ValueError) as error:
        parser.exit(1, f"{error}\n")
