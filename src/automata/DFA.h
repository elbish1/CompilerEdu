#pragma once

#include "automata/NFA.h"

namespace edu::automata {

struct DFA {
  int start = 0;
  std::unordered_set<int> accept;
  int stateCount = 0;
  std::vector<char> alphabet;

  // transitions[dfaState][symbol] = dfaState
  std::unordered_map<int, std::unordered_map<char, int>> trans;

  // For display/debug: mapping DFA state -> NFA subset
  std::unordered_map<int, std::unordered_set<int>> subset;
};

DFA ConvertToDFA(const NFA& nfa);
void PrintDFA(const DFA& dfa);

}  // namespace edu::automata

