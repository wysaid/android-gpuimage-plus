import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
from zipfile import ZipFile

from validate_aar_elf import EXPECTED_ABIS, validate_aar


@patch("validate_aar_elf.validate_shared_library", return_value=[])
class AarLibraryCoverageTest(unittest.TestCase):
    def stage_aar(self, name, libraries, omitted=()):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        aar = Path(workspace.name) / name
        with ZipFile(aar, "w") as archive:
            for abi in EXPECTED_ABIS:
                for library in libraries:
                    entry = f"jni/{abi}/{library}"
                    if entry not in omitted:
                        archive.writestr(entry, b"native library fixture")
        return aar

    def test_missing_library_is_reported_even_when_abi_is_present(self, _):
        missing = "jni/x86/libCGEExt.so"
        aar = self.stage_aar(
            "gpuimage-plus-3.2.2.aar",
            ("libCGE.so", "libCGEExt.so", "libffmpeg.so"),
            omitted=(missing,),
        )
        self.assertEqual(validate_aar(aar), [f"{aar.name}: missing required native library {missing}"])

    def test_full_variants_require_ffmpeg_for_every_abi(self, _):
        aar = self.stage_aar("gpuimage-plus-3.2.2-16k.aar", ("libCGE.so", "libCGEExt.so"))
        self.assertCountEqual(
            validate_aar(aar),
            [
                f"{aar.name}: missing required native library jni/{abi}/libffmpeg.so"
                for abi in EXPECTED_ABIS
            ],
        )

    def test_min_variants_accept_all_abis_without_ffmpeg(self, _):
        for suffix in ("-min", "-16k-min"):
            with self.subTest(suffix=suffix):
                aar = self.stage_aar(
                    f"gpuimage-plus-3.2.2{suffix}.aar", ("libCGE.so", "libCGEExt.so")
                )
                self.assertEqual(validate_aar(aar), [])


if __name__ == "__main__":
    unittest.main()
