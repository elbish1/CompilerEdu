#include "automata/NFA.h"

namespace edu::automata {

std::unordered_set<int> EpsilonClosure(const NFA& nfa, const std::unordered_set<int>& states) {
  std::unordered_set<int> closure = states;
  std::vector<int> stack;
  stack.reserve(states.size());
  for (int s : states) stack.push_back(s);

  while (!stack.empty()) {
    int u = stack.back();
    stack.pop_back();
    auto itFrom = nfa.trans.find(u);
    if (itFrom == nfa.trans.end()) continue;
    auto itE = itFrom->second.find(EPS);
    if (itE == itFrom->second.end()) continue;
    for (int v : itE->second) {
      if (!closure.count(v)) {
        closure.insert(v);
        stack.push_back(v);
      }
    }
  }
  return closure;
}

std::unordered_set<int> Move(const NFA& nfa, const std::unordered_set<int>& states, char sym) {
  std::unordered_set<int> out;
  for (int s : states) {
    auto itFrom = nfa.trans.find(s);
    if (itFrom == nfa.trans.end()) continue;
    auto it = itFrom->second.find(sym);
    if (it == itFrom->second.end()) continue;
    for (int v : it->second) out.insert(v);
  }
  return out;
}

std::vector<char> Alphabet(const NFA& nfa) {
  std::set<char> a;
  for (auto it = nfa.trans.begin(); it != nfa.trans.end(); ++it) {
    const auto& m = it->second;
    for (auto it2 = m.begin(); it2 != m.end(); ++it2) {
      char sym = it2->first;
      if (sym != EPS) a.insert(sym);
    }
  }
  return std::vector<char>(a.begin(), a.end());
}

void PrintNFA(const NFA& nfa) {
  std::cout << "NFA\n";
  std::cout << "States: 0.." << (nfa.stateCount - 1) << "\n";
  std::cout << "Start: " << nfa.start << "\n";
  std::cout << "Accept: {";
  bool first = true;
  for (int a : nfa.accept) {
    if (!first) std::cout << ", ";
    first = false;
    std::cout << a;
  }
  std::cout << "}\n";

  std::cout << "Transitions:\n";
  for (int s = 0; s < nfa.stateCount; ++s) {
    auto itFrom = nfa.trans.find(s);
    if (itFrom == nfa.trans.end()) continue;
    const auto& m = itFrom->second;
    for (auto it = m.begin(); it != m.end(); ++it) {
      char sym = it->first;
      const auto& tos = it->second;
      for (int t : tos) {
        std::cout << "  " << s << " -- ";
        if (sym == EPS) std::cout << "ε";
        else std::cout << sym;
        std::cout << " --> " << t << "\n";
      }
    }
  }
}

struct Prev {
  int prevState = -1;
  int prevIdx = -1;
};

std::vector<int> FindPathForString(const NFA& nfa, const std::string& input) {
  // BFS over (closureStateSet, idx) is heavy; for educational path, we BFS over concrete state sets with predecessors.
  using Set = std::unordered_set<int>;

  auto keyOf = [&](const Set& s) -> std::string {
    std::vector<int> v(s.begin(), s.end());
    std::sort(v.begin(), v.end());
    std::ostringstream oss;
    for (int x : v) oss << x << ",";
    return oss.str();
  };

  struct Node { Set st; int idx; };
  std::vector<Node> nodes;
  std::unordered_map<std::string, int> seen;  // key + idx
  std::vector<Prev> prev;

  Set startSet = EpsilonClosure(nfa, Set{nfa.start});
  nodes.push_back(Node{startSet, 0});
  prev.push_back(Prev{-1, -1});
  seen[keyOf(startSet) + "|" + std::to_string(0)] = 0;

  int goalNode = -1;
  for (size_t qi = 0; qi < nodes.size(); ++qi) {
    const auto cur = nodes[qi];
    if (cur.idx == static_cast<int>(input.size())) {
      for (int s : cur.st) {
        if (nfa.accept.count(s)) { goalNode = static_cast<int>(qi); break; }
      }
      if (goalNode != -1) break;
    }
    if (cur.idx >= static_cast<int>(input.size())) continue;
    char c = input[static_cast<size_t>(cur.idx)];
    Set moved = Move(nfa, cur.st, c);
    Set next = EpsilonClosure(nfa, moved);
    if (next.empty()) continue;
    std::string k = keyOf(next) + "|" + std::to_string(cur.idx + 1);
    if (seen.count(k)) continue;
    int nid = static_cast<int>(nodes.size());
    nodes.push_back(Node{next, cur.idx + 1});
    prev.push_back(Prev{static_cast<int>(qi), cur.idx});
    seen[k] = nid;
  }

  if (goalNode == -1) return {};

  // Reconstruct state-by-state path (choose representative states along the subset chain).
  // We will pick any accepting member at end and then pick any predecessor that can lead.
  // For simplicity, output representative state IDs along the subsets.
  std::vector<Set> subsetPath;
  for (int n = goalNode; n != -1; n = prev[static_cast<size_t>(n)].prevState) {
    subsetPath.push_back(nodes[static_cast<size_t>(n)].st);
  }
  std::reverse(subsetPath.begin(), subsetPath.end());

  std::vector<int> statePath;
  statePath.reserve(input.size() + 1);

  // Pick a deterministic representative: minimum state in subset.
  for (const auto& ss : subsetPath) {
    int best = *std::min_element(ss.begin(), ss.end());
    statePath.push_back(best);
  }
  if (statePath.size() != input.size() + 1) return {};
  return statePath;
}

}  // namespace edu::automata

