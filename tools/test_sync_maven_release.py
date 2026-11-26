import tempfile
import unittest
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path

from sync_maven_release import sync_release


class MavenReleaseSyncTest(unittest.TestCase):
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
        sync_release(self.stage_publication(beta), self.target, beta)
        sync_release(self.stage_publication(stable), self.target, stable)

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
            sync_release(staging, self.target, "3.2.2-beta1")
        self.assertFalse(self.target.exists())

    def test_late_publication_with_older_build_timestamp_does_not_roll_back_metadata(self):
        before_sync = datetime.now(timezone.utc).strftime("%Y%m%d%H%M%S")
        newer = self.stage_publication("3.2.3", "20261127020000")
        older = self.stage_publication("3.2.2", "20261127010000")
        sync_release(newer, self.target, "3.2.3")
        previous = self.published_metadata().findtext("versioning/lastUpdated")

        sync_release(older, self.target, "3.2.2")

        current = self.published_metadata().findtext("versioning/lastUpdated")
        self.assertGreaterEqual(previous, before_sync)
        self.assertGreaterEqual(current, previous)


if __name__ == "__main__":
    unittest.main()
