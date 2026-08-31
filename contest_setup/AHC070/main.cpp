// AHC070 post-contest implementation.
// Problem: https://atcoder.jp/contests/ahc070/tasks/ahc070_a
// Reference idea: https://atcoder.jp/contests/ahc070/editorial/24888
// Pre-published utility used here:
// https://github.com/Shibungi/heuristic-toolkit/blob/2d90ab3f4d7c05ad2434ec5ff0767fa07009c64b/library/timer/timer.hpp

#include <bits/stdc++.h>

#include "library/timer/timer.hpp"

using namespace std;

namespace {

constexpr int N = 100;
constexpr int M = 3;
constexpr int T = N * N;
constexpr int MAX_DIST = 2 * (N - 1);

// 未来ほど、今の距離場をそのまま信じすぎない。
// t_nowからdtターン後の怪異は 0.9995^dt 倍して評価する。
constexpr double FUTURE_DECAY = 0.9995;

// 既に札があるマスへもう一度置く手を、同評価なら少しだけ避ける。
// 公式危険度に比べて十分小さく、主目的を壊さないtie-break程度の値にする。
constexpr double OVERLAP_PENALTY = 0.25;

// 短い先読みを何度もやり直すreceding-horizon Beam。
// 10,000手を一度にBeamすると状態履歴が深くなりすぎるため、2手だけ確定して再探索する。
constexpr int MAX_LOOKAHEAD = 8;
constexpr int NORMAL_BEAM_WIDTH = 96;
constexpr int COMMIT_STEPS = 2;

struct Pos {
  int r = 0;
  int c = 0;
};

struct Move {
  int dr = 0;
  int dc = 0;
};

struct Input {
  int n = 0;
  int m = 0;
  array<Pos, T> target{};
};

Input read_input() {
  Input in;
  cin >> in.n >> in.m;
  for (Pos& p : in.target) cin >> p.r >> p.c;
  return in;
}

int cell_id(int r, int c) { return r * N + c; }

int manhattan(int id, int r, int c) {
  return abs(id / N - r) + abs(id % N - c);
}

// Beam中は同じ距離場へapply/undoを繰り返す。
// 変更されたセルだけjournalへ積み、枝を戻すときに逆順で復元する。
class SearchState {
 public:
  struct Snapshot {
    size_t journal_size = 0;
    Pos old_pos{};
    int old_time = 0;
    long long old_confirmed_cost = 0;
    double old_future_cost = 0.0;
    double old_overlap_cost = 0.0;
    int placed_cell = -1;
  };

 private:
  const Input& in_;
  const array<Move, M>& moves_;

  // dist[x] = 現在までに置いた札からセルxまでの最小マンハッタン距離。
  // 札の追加では距離は減るだけなので、BFS緩和とrollbackを利用できる。
  array<uint8_t, T> dist_{};
  array<uint16_t, T> visit_count_{};
  array<int, T> target_time_{};

  // step_cost[t][d] = floor(d * sqrt(t+1))。
  // 距離場を更新するhot loopでsqrt/floorを呼ばないための表。
  array<array<uint16_t, MAX_DIST + 1>, T> step_cost_{};
  array<double, T + 1> decay_pow_{};

  Pos pos_{};
  int time_ = 0;  // 次に処理する怪異の時刻
  long long confirmed_cost_ = 0;
  double future_cost_ = 0.0;
  double overlap_cost_ = 0.0;

  vector<pair<int, uint8_t>> journal_;
  array<int, T> bfs_queue_{};

  // dist[cell]を短くし、そのセルで発生する未来の怪異の見積もりも同時に更新する。
  void lower_distance(int cell, uint8_t new_dist) {
    const uint8_t old_dist = dist_[cell];
    if (new_dist >= old_dist) return;

    journal_.emplace_back(cell, old_dist);
    dist_[cell] = new_dist;

    const int haunting_time = target_time_[cell];
    if (haunting_time >= time_) {
      const int old_danger = step_cost_[haunting_time][old_dist];
      const int new_danger = step_cost_[haunting_time][new_dist];
      future_cost_ +=
          double(new_danger - old_danger) * decay_pow_[haunting_time - time_];
    }
  }

  // 新しい札を距離0のsourceとして、改善が起きる領域だけBFSする。
  void relax_from(int source) {
    if (dist_[source] == 0) return;

    int head = 0;
    int tail = 0;
    lower_distance(source, 0);
    bfs_queue_[tail++] = source;

    constexpr int DR[4] = {-1, 1, 0, 0};
    constexpr int DC[4] = {0, 0, -1, 1};
    while (head < tail) {
      const int cell = bfs_queue_[head++];
      const int r = cell / N;
      const int c = cell % N;
      const uint8_t next_dist = uint8_t(dist_[cell] + 1);
      for (int dir = 0; dir < 4; ++dir) {
        const int nr = r + DR[dir];
        const int nc = c + DC[dir];
        if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;
        const int next = cell_id(nr, nc);
        if (next_dist < dist_[next]) {
          lower_distance(next, next_dist);
          bfs_queue_[tail++] = next;
        }
      }
    }
  }

 public:
  SearchState(const Input& in, const array<Move, M>& moves)
      : in_(in), moves_(moves) {
    dist_.fill(MAX_DIST);
    visit_count_.fill(0);
    journal_.reserve(T * 2);

    decay_pow_[0] = 1.0;
    for (int dt = 1; dt <= T; ++dt) {
      decay_pow_[dt] = decay_pow_[dt - 1] * FUTURE_DECAY;
    }

    for (int t = 0; t < T; ++t) {
      const int target = cell_id(in_.target[t].r, in_.target[t].c);
      target_time_[target] = t;
      const double weight = sqrt(double(t + 1));
      for (int d = 0; d <= MAX_DIST; ++d) {
        step_cost_[t][d] = uint16_t(double(d) * weight);
      }
    }

    // 札がまだ無い仮状態では全セルを最大距離198としておく。
    // 1手目の札を置くと全セルが正しい距離以下へ緩和されるため、評価時には正確な距離場になる。
    for (int t = 0; t < T; ++t) {
      future_cost_ += double(step_cost_[t][MAX_DIST]) * decay_pow_[t];
    }
  }

  int time() const { return time_; }
  long long confirmed_cost() const { return confirmed_cost_; }
  double value() const {
    return double(confirmed_cost_) + future_cost_ + overlap_cost_;
  }

  Snapshot apply(int action) {
    Snapshot snapshot{journal_.size(), pos_, time_, confirmed_cost_, future_cost_,
                      overlap_cost_, -1};

    const Move mv = moves_[action];
    pos_.r = (pos_.r + mv.dr) % N;
    pos_.c = (pos_.c + mv.dc) % N;
    const int placed = cell_id(pos_.r, pos_.c);
    snapshot.placed_cell = placed;

    if (visit_count_[placed] > 0) overlap_cost_ += OVERLAP_PENALTY;
    ++visit_count_[placed];

    // 札を置いた直後の距離場で、時刻time_の公式危険度を確定する。
    relax_from(placed);
    const int target = cell_id(in_.target[time_].r, in_.target[time_].c);
    const int danger = step_cost_[time_][dist_[target]];
    confirmed_cost_ += danger;

    // future_cost_は現在時刻を指数0として持つ。
    // 今確定した項を外し、次時刻が指数0になるようDECAYで割る。
    if (time_ + 1 < T) {
      future_cost_ = (future_cost_ - double(danger)) / FUTURE_DECAY;
    } else {
      future_cost_ = 0.0;
    }
    ++time_;
    return snapshot;
  }

  void undo(const Snapshot& snapshot) {
    --visit_count_[snapshot.placed_cell];
    while (journal_.size() > snapshot.journal_size) {
      const auto [cell, old_dist] = journal_.back();
      journal_.pop_back();
      dist_[cell] = old_dist;
    }
    pos_ = snapshot.old_pos;
    time_ = snapshot.old_time;
    confirmed_cost_ = snapshot.old_confirmed_cost;
    future_cost_ = snapshot.old_future_cost;
    overlap_cost_ = snapshot.old_overlap_cost;
  }

  // 確定prefixの変更履歴は、以後undoしないので破棄してよい。
  void clear_journal() { journal_.clear(); }

  // 時間切れ間際のO(1) greedy。候補位置が「今の怪異」へ与える距離だけを比較する。
  int greedy_action() const {
    const Pos target = in_.target[time_];
    int best_action = 0;
    double best_value = numeric_limits<double>::infinity();
    for (int action = 0; action < M; ++action) {
      const int nr = (pos_.r + moves_[action].dr) % N;
      const int nc = (pos_.c + moves_[action].dc) % N;
      const int placed = cell_id(nr, nc);
      const int current_target = cell_id(target.r, target.c);
      const int d = min<int>(dist_[current_target], manhattan(placed, target.r, target.c));
      const double overlap = visit_count_[placed] ? OVERLAP_PENALTY : 0.0;
      const double candidate = double(step_cost_[time_][d]) + overlap;
      if (candidate < best_value) {
        best_value = candidate;
        best_action = action;
      }
    }
    return best_action;
  }
};

struct BeamNode {
  array<uint8_t, MAX_LOOKAHEAD> actions{};
  uint8_t length = 0;
  double value = 0.0;
};

struct BeamResult {
  BeamNode best{};
  uint64_t expanded_nodes = 0;
};

// 層ごとに評価値が小さい上位width本だけを残す、最小化Beam。
// State本体はコピーせず、rootから短いAction列をapply/undoして再現する。
BeamResult beam_lookahead(SearchState& state, int depth, int width,
                          const ht::Timer& timer, double stop_at_sec) {
  vector<BeamNode> current(1);
  vector<BeamNode> next;
  next.reserve(size_t(width) * M);
  BeamResult result;

  for (int layer = 0; layer < depth; ++layer) {
    next.clear();
    bool interrupted = false;

    for (size_t node_index = 0; node_index < current.size(); ++node_index) {
      if ((node_index & 31U) == 0U && timer.elapsed_exact() >= stop_at_sec) {
        interrupted = true;
        break;
      }

      const BeamNode& node = current[node_index];
      array<SearchState::Snapshot, MAX_LOOKAHEAD> prefix_snapshots{};
      for (int i = 0; i < node.length; ++i) {
        prefix_snapshots[i] = state.apply(node.actions[i]);
      }

      for (int action = 0; action < M; ++action) {
        const SearchState::Snapshot child_snapshot = state.apply(action);
        BeamNode child = node;
        child.actions[child.length++] = uint8_t(action);
        child.value = state.value();
        next.push_back(child);
        ++result.expanded_nodes;
        state.undo(child_snapshot);
      }

      for (int i = int(node.length) - 1; i >= 0; --i) {
        state.undo(prefix_snapshots[i]);
      }
    }

    // 中断した層は候補を一部しか生成していないため捨て、直前の完成した層を使う。
    if (interrupted || next.empty()) break;

    auto better = [](const BeamNode& lhs, const BeamNode& rhs) {
      return lhs.value < rhs.value;
    };
    if (int(next.size()) > width) {
      nth_element(next.begin(), next.begin() + width, next.end(), better);
      next.resize(width);
    }
    current.swap(next);
  }

  result.best = *min_element(current.begin(), current.end(),
                            [](const BeamNode& lhs, const BeamNode& rhs) {
                              return lhs.value < rhs.value;
                            });
  return result;
}

}  // namespace

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  const Input in = read_input();

  // 盤面を広く巡回しやすい固定ベクトル。行列の入れ替えに近い2本と横方向の1本を使う。
  // ベクトル探索と行動列探索を同時に行うと探索が薄くなるため、今回は行動列Beamへ集中する。
  const array<Move, M> moves{{Move{17, 19}, Move{19, 17}, Move{0, 83}}};

  ht::Timer timer(1.90, 0.06);
  SearchState state(in, moves);
  vector<int> answer;
  answer.reserve(T);
  uint64_t expanded_nodes = 0;

  while (state.time() < T) {
    const double elapsed = timer.elapsed_exact();

    // 残り時間が少なければ、Beamを打ち切って軽いgreedyで必ず10,000手を完成させる。
    if (elapsed >= 1.72) {
      while (state.time() < T) {
        const int action = state.greedy_action();
        state.apply(action);
        state.clear_journal();
        answer.push_back(action);
      }
      break;
    }

    // 後半ほど1回の探索を軽くし、出力用の余裕を残す。
    const int depth = (elapsed < 1.30) ? MAX_LOOKAHEAD : 6;
    const int width = (elapsed < 1.30) ? NORMAL_BEAM_WIDTH : 48;
    BeamResult result = beam_lookahead(state, min(depth, T - state.time()), width,
                                       timer, 1.72);
    expanded_nodes += result.expanded_nodes;

    if (result.best.length == 0) continue;
    const int commit = min<int>({COMMIT_STEPS, result.best.length, T - state.time()});
    for (int i = 0; i < commit; ++i) {
      const int action = result.best.actions[i];
      state.apply(action);
      state.clear_journal();
      answer.push_back(action);
    }
  }

  assert(state.time() == T);
  assert(int(answer.size()) == T);

  for (const Move mv : moves) cout << mv.dr << ' ' << mv.dc << '\n';
  for (const int action : answer) cout << action << '\n';

  cerr << "confirmed cost: " << state.confirmed_cost() << '\n';
  cerr << "expanded nodes: " << expanded_nodes << '\n';
  cerr << "elapsed: " << timer.elapsed_exact() << " sec\n";
  return 0;
}
