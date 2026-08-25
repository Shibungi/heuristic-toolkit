#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace ht {

inline std::vector<int> bfs(const std::vector<std::vector<int>>& g, const std::vector<int>& sources) {
  std::vector<int> dist(g.size(), -1); std::queue<int> q;
  for (int s : sources) if (dist[s] == -1) dist[s] = 0, q.push(s);
  while (!q.empty()) { int v = q.front(); q.pop(); for (int u : g[v]) if (dist[u] == -1) dist[u] = dist[v] + 1, q.push(u); }
  return dist;
}
inline std::vector<long long> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& g, int source) {
  constexpr long long INF = std::numeric_limits<long long>::max() / 4;
  std::vector<long long> dist(g.size(), INF);
  using Q = std::pair<long long, int>; std::priority_queue<Q, std::vector<Q>, std::greater<Q>> pq;
  dist[source] = 0; pq.push({0, source});
  while (!pq.empty()) { auto [d, v] = pq.top(); pq.pop(); if (d != dist[v]) continue; for (auto [u, w] : g[v]) if (d + w < dist[u]) dist[u] = d + w, pq.push({dist[u], u}); }
  return dist;
}

}  // namespace ht
