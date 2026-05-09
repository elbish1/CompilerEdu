#pragma once

#include "automata/NFA.h"

namespace edu::automata {

// Supported regex for educational use:
// - literal characters: a..z A..Z 0..9 (any single non-operator)
// - union: |
// - concatenation: implicit
// - star: *
// - parentheses: ( )
//
// Example: (a|b)*abb
NFA RegexToNFA(const std::string& regex);

}  // namespace edu::automata

