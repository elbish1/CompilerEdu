#pragma once

#include "compiler/IR.h"

namespace edu::compiler {

class Optimizer {
 public:
  // Machine-independent optimizations
  static std::vector<IRInstr> ConstantFolding(const std::vector<IRInstr>& ir);
  static std::vector<IRInstr> DeadCodeElim(const std::vector<IRInstr>& ir);

  // Machine-dependent (simple simulation)
  static std::vector<IRInstr> RemoveRedundant(const std::vector<IRInstr>& ir);
};

}  // namespace edu::compiler

