import pathlib
import subprocess
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]


class PastAhcExamplesTest(unittest.TestCase):
    def test_one_example_per_library_header_compiles(self):
        headers = sorted((ROOT / "library").rglob("*.hpp"))
        examples = sorted((ROOT / "examples" / "past_ahc").glob("*.cpp"))
        self.assertEqual(len(headers), 17)
        self.assertEqual(len(examples), len(headers))
        for source in examples:
            with self.subTest(source=source.name):
                subprocess.run(
                    ["g++", "-std=c++20", "-O0", "-I", str(ROOT), "-fsyntax-only", str(source)],
                    check=True,
                    capture_output=True,
                    text=True,
                )


if __name__ == "__main__":
    unittest.main()
