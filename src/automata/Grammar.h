#pragma once

#include "automata/NFA.h"

namespace edu::automata {

struct Production {
  std::string lhs;   // e.g., A0
  std::string rhs;   // e.g., aA1 or ε
};

struct RegularGrammar {
  std::string startSymbol;
  std::vector<Production> prods;
};

RegularGrammar NfaToRegularGrammar(const NFA& nfa);
void PrintGrammar(const RegularGrammar& g);

// Derivation using the NFA path for a specific string (educational simulation).
std::vector<std::string> LeftmostDerivation(const NFA& nfa, const std::string& s);
std::vector<std::string> RightmostDerivation(const NFA& nfa, const std::string& s);

}  // namespace edu::automata

