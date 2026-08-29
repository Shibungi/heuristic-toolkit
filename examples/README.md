# Examples

`permutation_sa.cpp` is a complete, deliberately small delta-evaluation example. Build:

```bash
environment/compile debug examples/permutation_sa.cpp /tmp/permutation_sa
printf '6\n5 1 4 2 0 3\n' | /tmp/permutation_sa
```

Contest templates remain in `templates/`; examples are for tests and reading, not for
copying as a framework.

`past_ahc/` には、17個のライブラリヘッダそれぞれについて、AHC002・004・007・009・021
の実際の入出力へ接続した日本語コメント付きbaselineがあります。対応表とコンパイル方法は
[`past_ahc/README.md`](past_ahc/README.md)を参照してください。
