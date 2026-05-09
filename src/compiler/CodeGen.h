#pragma once

#include "compiler/IR.h"

namespace edu::compiler {

class CodeGen {
 public:
  // Pseudo target instructions (text)
  static std::vector<std::string> Generate(const std::vector<IRInstr>& ir);
};

inline void PrintTarget(const std::vector<std::string>& code) {
  for (size_t i = 0; i < code.size(); ++i) {
    std::cout << i << ": " << code[i] << "\n";
  }
}

}  // namespace edu::compiler

