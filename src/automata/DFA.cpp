#include "automata/DFA.h"

namespace edu::automata {

static std::string KeyOf(const std::unordered_set<int>& s) {
  std::vector<int> v(s.begin(), s.end());
  std::sort(v.begin(), v.end());
  std::ostringstream oss;
  for (int x : v) oss << x << ",";
  return oss.str();
}

DFA ConvertToDFA(const NFA& nfa) {
  DFA dfa;
  dfa.alphabet = Alphabet(nfa);

  using Set = std::unordered_set<int>;
  std::unordered_map<std::string, int> id;
  std::vector<Set> states;

  Set startSet = EpsilonClosure(nfa, Set{nfa.start});
  id[KeyOf(startSet)] = 0;
  states.push_back(startSet);
  dfa.subset[0] = startSet;
  dfa.start = 0;
  dfa.stateCount = 1;

  for (size_t i = 0; i < states.size(); ++i) {
    int curId = static_cast<int>(i);
    const Set& cur = states[i];

    bool isAcc = false;
    for (int s : cur) { if (nfa.accept.count(s)) { isAcc = true; break; } }
    if (isAcc) dfa.accept.insert(curId);

    for (char a : dfa.alphabet) {
      Set moved = Move(nfa, cur, a);
      Set nxt = EpsilonClosure(nfa, moved);
      if (nxt.empty()) continue;
      std::string k = KeyOf(nxt);
      int nxtId = -1;
      auto it = id.find(k);
      if (it == id.end()) {
        nxtId = dfa.stateCount++;
        id[k] = nxtId;
        states.push_back(nxt);
        dfa.subset[nxtId] = nxt;
      } else {
        nxtId = it->second;
      }
      dfa.trans[curId][a] = nxtId;
    }
  }

  return dfa;
}

void PrintDFA(const DFA& dfa) {
  std::cout << "DFA\n";
  std::cout << "States: 0.." << (dfa.stateCount - 1) << "\n";
  std::cout << "Start: " << dfa.start << "\n";
  std::cout << "Accept: {";
  bool first = true;
  for (int a : dfa.accept) {
    if (!first) std::cout << ", ";
    first = false;
    std::cout << a;
  }
  std::cout << "}\n";

  std::cout << "Alphabet: {";
  for (size_t i = 0; i < dfa.alphabet.size(); ++i) {
    if (i) std::cout << ", ";
    std::cout << dfa.alphabet[i];
  }
  std::cout << "}\n";

  std::cout << "Transitions:\n";
  for (int s = 0; s < dfa.stateCount; ++s) {
    auto it = dfa.trans.find(s);
    if (it == dfa.trans.end()) continue;
    const auto& m = it->second;
    for (auto it2 = m.begin(); it2 != m.end(); ++it2) {
      char sym = it2->first;
      int to = it2->second;
      std::cout << "  " << s << " -- " << sym << " --> " << to << "\n";
    }
  }

  std::cout << "State subsets:\n";
  for (int s = 0; s < dfa.stateCount; ++s) {
    auto it = dfa.subset.find(s);
    if (it == dfa.subset.end()) continue;
    std::vector<int> v(it->second.begin(), it->second.end());
    std::sort(v.begin(), v.end());
    std::cout << "  " << s << " = {";
    for (size_t i = 0; i < v.size(); ++i) {
      if (i) std::cout << ", ";
      std::cout << v[i];
    }
    std::cout << "}\n";
  }
}

}  // namespace edu::automata

