import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parents[1] / "tools"))
from bundle import BundleError, Bundler


class BundleTests(unittest.TestCase):
    def test_nested_local_includes_and_system_include(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "detail").mkdir()
            (root / "detail" / "value.hpp").write_text("#pragma once\nconstexpr int value = 42;\n")
            (root / "util.hpp").write_text('#pragma once\n#include "detail/value.hpp"\n')
            (root / "main.cpp").write_text('#include <bits/stdc++.h>\n#include "util.hpp"\n#include "util.hpp"\nint main(){return value;}\n')
            result = Bundler([]).bundle(root / "main.cpp")
            self.assertIn("#include <bits/stdc++.h>", result)
            self.assertNotIn('#include "util.hpp"', result)
            self.assertEqual(result.count("constexpr int value = 42;"), 1)
            self.assertIn("skipped duplicate local include", result)

    def test_include_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory); include = root / "include"; include.mkdir()
            (include / "shared.hpp").write_text("#pragma once\nstruct Shared {};\n")
            (root / "main.cpp").write_text('#include "shared.hpp"\n')
            self.assertIn("struct Shared", Bundler([include]).bundle(root / "main.cpp"))

    def test_missing_local_include_is_error(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "main.cpp"
            source.write_text('#include "missing.hpp"\n')
            with self.assertRaises(BundleError):
                Bundler([]).bundle(source)

    def test_cycle_is_bounded(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "a.hpp").write_text('#include "b.hpp"\nstruct A {};\n')
            (root / "b.hpp").write_text('#include "a.hpp"\nstruct B {};\n')
            (root / "main.cpp").write_text('#include "a.hpp"\n')
            result = Bundler([]).bundle(root / "main.cpp")
            self.assertIn("skipped cyclic local include", result)
            self.assertEqual(result.count("struct A"), 1)


if __name__ == "__main__":
    unittest.main()
