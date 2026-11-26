import shutil
import tempfile
import unittest
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path

from sync_maven_release import sync_release


class MavenReleaseSyncTest(unittest.TestCase):
    source_commit = "a" * 40

    def setUp(self):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.root = Path(workspace.name)
        self.target = self.root / "published"

    def stage_publication(self, version, timestamp="20261127010000"):
        source = self.root / version
        metadata = ET.Element("metadata")
        ET.SubElement(metadata, "artifactId").text = "gpuimage-plus"
        versioning = ET.SubElement(metadata, "versioning")
        ET.SubElement(versioning, "lastUpdated").text = timestamp
        versions = ET.SubElement(versioning, "versions")

        for suffix in ("", "-min", "-16k", "-16k-min"):
            variant = version + suffix
            ET.SubElement(versions, "version").text = variant
            directory = source / variant
            directory.mkdir(parents=True)
            for extension in ("aar", "pom"):
                filename = f"gpuimage-plus-{variant}.{extension}"
                (directory / filename).write_text(variant, encoding="utf-8")

        ET.ElementTree(metadata).write(source / "maven-metadata.xml", encoding="utf-8")
        return source

    def published_metadata(self):
        return ET.parse(self.target / "maven-metadata.xml")

    def test_beta_and_stable_coordinates_coexist(self):
        beta = "3.2.2-beta1"
        stable = "3.2.2"
        sync_release(self.stage_publication(beta), self.target, beta, self.source_commit)
        sync_release(self.stage_publication(stable), self.target, stable, self.source_commit)

        versions = self.published_metadata().findall("versioning/versions/version")
        self.assertEqual(
            {version.text for version in versions},
            {base + suffix for base in (beta, stable) for suffix in ("", "-min", "-16k", "-16k-min")},
        )
        for base in (beta, stable):
            path = self.target / base / f"gpuimage-plus-{base}.aar"
            self.assertEqual(path.read_text(encoding="utf-8"), base)

    def test_staged_stable_coordinates_are_rejected_for_beta_tag(self):
        staging = self.stage_publication("3.2.2")

        with self.assertRaisesRegex(ValueError, "Staged Maven versions do not match 3.2.2-beta1"):
            sync_release(staging, self.target, "3.2.2-beta1", self.source_commit)
        self.assertFalse(self.target.exists())

    def test_late_publication_with_older_build_timestamp_does_not_roll_back_metadata(self):
        before_sync = datetime.now(timezone.utc).strftime("%Y%m%d%H%M%S")
        newer = self.stage_publication("3.2.3", "20261127020000")
        older = self.stage_publication("3.2.2", "20261127010000")
        sync_release(newer, self.target, "3.2.3", self.source_commit)
        previous = self.published_metadata().findtext("versioning/lastUpdated")

        sync_release(older, self.target, "3.2.2", self.source_commit)

        current = self.published_metadata().findtext("versioning/lastUpdated")
        self.assertGreaterEqual(previous, before_sync)
        self.assertGreaterEqual(current, previous)

    def test_rerun_from_same_commit_keeps_published_files(self):
        version = "3.2.2"
        staging = self.stage_publication(version)
        sync_release(staging, self.target, version, self.source_commit)

        artifact = Path(version) / f"gpuimage-plus-{version}.aar"
        (staging / artifact).write_text("rebuilt AAR", encoding="utf-8")
        previous_metadata = (self.target / "maven-metadata.xml").read_bytes()

        sync_release(staging, self.target, version, self.source_commit)

        self.assertEqual((self.target / artifact).read_text(encoding="utf-8"), version)
        self.assertEqual((self.target / "maven-metadata.xml").read_bytes(), previous_metadata)

    def test_rerun_from_a_different_commit_is_rejected(self):
        version = "3.2.2"
        staging = self.stage_publication(version)
        sync_release(staging, self.target, version, self.source_commit)

        with self.assertRaisesRegex(ValueError, "published from a different commit"):
            sync_release(staging, self.target, version, "b" * 40)

    def test_legacy_publication_still_requires_identical_artifacts(self):
        version = "3.2.1"
        staging = self.stage_publication(version)
        shutil.copytree(staging, self.target)
        artifact = Path(version) / f"gpuimage-plus-{version}.aar"
        (staging / artifact).write_text("rebuilt AAR", encoding="utf-8")

        with self.assertRaisesRegex(ValueError, "published with different artifacts"):
            sync_release(staging, self.target, version, self.source_commit)
        self.assertEqual((self.target / artifact).read_text(encoding="utf-8"), version)


if __name__ == "__main__":
    unittest.main()
