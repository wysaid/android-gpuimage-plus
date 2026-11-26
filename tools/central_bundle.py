#!/usr/bin/env python3
"""Package four gpuimage-plus Maven variants for manual Central Portal upload."""

import argparse
import hashlib
import re
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path
from zipfile import ZIP_DEFLATED, BadZipFile, ZipFile

MAVEN_PATH = Path("org/wysaid/gpuimage-plus")
VARIANTS = ("", "-min", "-16k", "-16k-min")
POM_NAMESPACE = {"": "http://maven.apache.org/POM/4.0.0"}


def release_artifacts(repository, version):
    for suffix in VARIANTS:
        variant = version + suffix
        directory = repository / MAVEN_PATH / variant
        prefix = directory / f"gpuimage-plus-{variant}"
        aar = Path(f"{prefix}.aar")
        pom = Path(f"{prefix}.pom")
        sources = Path(f"{prefix}-sources.jar")
        javadoc = Path(f"{prefix}-javadoc.jar")
        for artifact in (aar, pom, sources, javadoc):
            if not artifact.is_file() or artifact.stat().st_size == 0:
                raise ValueError(f"Missing or empty artifact: {artifact}")

        project = ET.parse(pom).getroot()
        for field, expected in (
            ("groupId", "org.wysaid"),
            ("artifactId", "gpuimage-plus"),
            ("version", variant),
            ("packaging", "aar"),
        ):
            if project.findtext(field, namespaces=POM_NAMESPACE) != expected:
                raise ValueError(f"{pom}: expected {field}={expected}")

        for jar, extension in ((sources, ".java"), (javadoc, ".html")):
            with ZipFile(jar) as archive:
                if not any(
                    name.startswith("org/wysaid/") and name.endswith(extension)
                    for name in archive.namelist()
                ):
                    raise ValueError(f"{jar}: missing org.wysaid {extension} files")

        yield from (aar, pom, sources, javadoc)


def write_bundle(repository, version, output, signing_key):
    artifacts = list(release_artifacts(repository, version))
    with tempfile.TemporaryDirectory(dir=output.parent) as temporary:
        staging = Path(temporary)
        bundle = staging / "bundle.zip"
        with ZipFile(bundle, mode="w", compression=ZIP_DEFLATED) as archive:
            for artifact in artifacts:
                name = artifact.relative_to(repository).as_posix()
                archive.write(artifact, name)
                content = artifact.read_bytes()
                for algorithm in ("md5", "sha1"):
                    checksum = hashlib.new(algorithm, content, usedforsecurity=False)
                    archive.writestr(f"{name}.{algorithm}", checksum.hexdigest())
                if signing_key:
                    signature = staging / f"{artifact.name}.asc"
                    subprocess.run(
                        [
                            "gpg", "--local-user", signing_key, "--armor", "--detach-sign",
                            "--output", str(signature), str(artifact),
                        ],
                        check=True,
                    )
                    archive.write(signature, name + ".asc")
        bundle.replace(output)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("repository", type=Path, help="Maven repository root containing org/wysaid")
    parser.add_argument("version", help="Regular release version (X.Y.Z)")
    parser.add_argument("output", type=Path, help="Output ZIP file")
    parser.add_argument("--signing-key", help="Local GPG key fingerprint for the Portal upload")
    args = parser.parse_args()
    if not re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", args.version):
        parser.error("version must be X.Y.Z")
    try:
        write_bundle(args.repository, args.version, args.output, args.signing_key)
    except (BadZipFile, ET.ParseError, OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Error: {error}\n")
    print(f"Created {'signed' if args.signing_key else 'unsigned'} Maven bundle: {args.output}")


if __name__ == "__main__":
    main()
