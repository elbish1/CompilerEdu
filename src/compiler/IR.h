#pragma once

#include "compiler/AST.h"

namespace edu::compiler {

enum class IROp {
  Add,
  Sub,
  Mul,
  Div,
  Mov,     // res = arg1
  Print,   // print arg1
};

inline const char* ToString(IROp op) {
  switch (op) {
    case IROp::Add: return "ADD";
    case IROp::Sub: return "SUB";
    case IROp::Mul: return "MUL";
    case IROp::Div: return "DIV";
    case IROp::Mov: return "MOV";
    case IROp::Print: return "PRINT";
  }
  return "???";
}

struct IRInstr {
  IROp op{};
  std::string res;
  std::string arg1;
  std::string arg2;

  bool HasSideEffect() const { return op == IROp::Print; }
};

class IRGen {
 public:
  std::vector<IRInstr> Generate(const ASTNode* root);

 private:
  std::string NewTemp();
  std::string GenExpr(const ASTNode* n);
  void GenStmt(const ASTNode* n);

  std::vector<IRInstr> out_;
  int tempId_ = 0;
};

inline void PrintIR(const std::vector<IRInstr>& ir) {
  for (size_t i = 0; i < ir.size(); ++i) {
    const auto& in = ir[i];
    std::cout << i << ": ";
    if (in.op == IROp::Print) {
      std::cout << "print " << in.arg1 << "\n";
      continue;
    }
    if (in.op == IROp::Mov) {
      std::cout << in.res << " = " << in.arg1 << "\n";
      continue;
    }
    std::cout << in.res << " = " << in.arg1 << " " << ToString(in.op) << " " << in.arg2 << "\n";
  }
}

}  // namespace edu::compiler

