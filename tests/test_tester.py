import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parents[1] / "tester"))
from common import extract_score, parse_cases, percentile


class TesterTests(unittest.TestCase):
    def test_cases(self):
        self.assertEqual(parse_cases("0-2,5,2"), [0, 1, 2, 5])
    def test_score(self):
        self.assertEqual(extract_score("Score = 10\nScore=20", r"Score\s*=\s*([0-9]+)"), 20)
    def test_percentile(self):
        self.assertEqual(percentile([1, 2, 3, 4, 5], 0.5), 3)


if __name__ == "__main__": unittest.main()
