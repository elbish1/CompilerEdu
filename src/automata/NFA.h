#pragma once

#include "common/Types.h"

namespace edu::automata {

static constexpr char EPS = '\0';

struct NFATransition {
  int from = 0;
  int to = 0;
  char symbol = EPS;  // EPS for epsilon
};

struct NFA {
  int start = 0;
  std::unordered_set<int> accept;
  int stateCount = 0;

  // transitions[from][symbol] -> set(to)
  std::unordered_map<int, std::unordered_map<char, std::unordered_set<int>>> trans;

  int NewState() { return stateCount++; }
  void AddEdge(int from, int to, char sym) { trans[from][sym].insert(to); }
};

std::unordered_set<int> EpsilonClosure(const NFA& nfa, const std::unordered_set<int>& states);
std::unordered_set<int> Move(const NFA& nfa, const std::unordered_set<int>& states, char sym);
std::vector<char> Alphabet(const NFA& nfa);

void PrintNFA(const NFA& nfa);

// Find one accepting path for a concrete string (ignoring epsilon labels in output).
// Returns sequence of states of length (input.size()+1) if found, else empty.
std::vector<int> FindPathForString(const NFA& nfa, const std::string& input);

}  // namespace edu::automata

