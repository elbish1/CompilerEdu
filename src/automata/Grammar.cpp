#include "automata/Grammar.h"

namespace edu::automata {

static std::string NT(int state) { return "A" + std::to_string(state); }

RegularGrammar NfaToRegularGrammar(const NFA& nfa) {
  RegularGrammar g;
  g.startSymbol = NT(nfa.start);

  // For each labeled transition: Aq -> a Ap
  for (auto it = nfa.trans.begin(); it != nfa.trans.end(); ++it) {
    int from = it->first;
    const auto& m = it->second;
    for (auto it2 = m.begin(); it2 != m.end(); ++it2) {
      char sym = it2->first;
      if (sym == EPS) continue;
      const auto& tos = it2->second;
      for (int to : tos) {
        g.prods.push_back(Production{NT(from), std::string(1, sym) + NT(to)});
      }
    }
  }

  // Accepting states add epsilon: Aq -> ε
  for (int a : nfa.accept) {
    g.prods.push_back(Production{NT(a), "ε"});
  }
  return g;
}

void PrintGrammar(const RegularGrammar& g) {
  std::cout << "Regular Grammar (from NFA)\n";
  std::cout << "Start: " << g.startSymbol << "\n";
  for (const auto& p : g.prods) {
    std::cout << "  " << p.lhs << " -> " << p.rhs << "\n";
  }
}

static std::vector<std::string> DeriveUsingPath(const NFA& nfa, const std::string& s) {
  // Use representative path (educational).
  std::vector<int> path = FindPathForString(nfa, s);
  if (path.empty()) return {};

  std::vector<std::string> steps;
  std::string cur = NT(nfa.start);
  steps.push_back(cur);

  for (size_t i = 0; i < s.size(); ++i) {
    // Replace current nonterminal with terminal + next nonterminal
    int from = path[i];
    int to = path[i + 1];
    std::string rep = std::string(1, s[i]) + NT(to);
    cur = rep;  // Regular grammar here produces a single nonterminal at the right end
    (void)from;
    steps.push_back(cur);
  }

  // ε step if accept reachable
  cur = cur;
  // Replace trailing nonterminal with ε
  // Form is: <terminals> A_k. We drop A_k.
  // Since our derivation string stores only suffix form, it's already terminals + nonterminal.
  // Remove last nonterminal part.
  // Find last 'A' and cut.
  size_t posA = cur.rfind('A');
  if (posA != std::string::npos) cur = cur.substr(0, posA);
  steps.push_back(cur);  // epsilon applied
  return steps;
}

std::vector<std::string> LeftmostDerivation(const NFA& nfa, const std::string& s) {
  return DeriveUsingPath(nfa, s);
}

std::vector<std::string> RightmostDerivation(const NFA& nfa, const std::string& s) {
  // For a right-linear grammar with a single nonterminal, leftmost == rightmost.
  return DeriveUsingPath(nfa, s);
}

}  // namespace edu::automata

