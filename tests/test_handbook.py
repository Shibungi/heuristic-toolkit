import pathlib
import unittest
from html.parser import HTMLParser


ROOT = pathlib.Path(__file__).resolve().parents[1]


class LibraryGuideParser(HTMLParser):
    def __init__(self):
        super().__init__()
        self.ids = set()
        self.entries = []
        self.nav_targets = []
        self.local_assets = []
        self.external_assets = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if "id" in attrs:
            self.ids.add(attrs["id"])
        classes = set(attrs.get("class", "").split())
        if tag == "article" and "library-entry" in classes:
            self.entries.append(attrs.get("id"))
        if tag == "a" and attrs.get("href", "").startswith("#"):
            self.nav_targets.append(attrs["href"][1:])
        for key in ("href", "src"):
            value = attrs.get(key)
            if not value:
                continue
            if value.startswith(("http://", "https://", "//")):
                self.external_assets.append(value)
            elif not value.startswith(("#", "mailto:")):
                self.local_assets.append(value)


class HandbookTest(unittest.TestCase):
    def setUp(self):
        html = (ROOT / "handbook/library.html").read_text(encoding="utf-8")
        self.parser = LibraryGuideParser()
        self.parser.feed(html)

    def test_all_library_headers_are_documented(self):
        headers = list((ROOT / "library").rglob("*.hpp"))
        self.assertEqual(len(headers), 17)
        self.assertEqual(len(self.parser.entries), len(headers))
        self.assertEqual(len(set(self.parser.entries)), len(self.parser.entries))

    def test_internal_navigation_targets_exist(self):
        missing = set(self.parser.nav_targets) - self.parser.ids
        self.assertEqual(missing, set())

    def test_assets_exist_and_page_is_offline(self):
        self.assertEqual(self.parser.external_assets, [])
        handbook = ROOT / "handbook"
        for relative in self.parser.local_assets:
            self.assertTrue((handbook / relative).resolve().exists(), relative)

    def test_tree_beam_explainer_hooks_exist(self):
        required = {"beam-tree-svg", "state-cursor", "beam-step", "beam-play", "beam-event-progress"}
        self.assertTrue(required <= self.parser.ids)


if __name__ == "__main__":
    unittest.main()
