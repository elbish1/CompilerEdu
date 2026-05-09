#include "compiler/CodeGen.h"

namespace edu::compiler {

static bool IsLiteral(const std::string& s) {
  if (s.empty()) return false;
  size_t i = 0;
  if (s[0] == '-') i = 1;
  if (i >= s.size()) return false;
  for (; i < s.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
  }
  return true;
}

static std::string LoadOperand(const std::string& x) {
  if (IsLiteral(x)) return "LOADI " + x;
  return "LOAD " + x;
}

std::vector<std::string> CodeGen::Generate(const std::vector<IRInstr>& ir) {
  std::vector<std::string> out;
  for (const auto& in : ir) {
    if (in.op == IROp::Print) {
      out.push_back(LoadOperand(in.arg1));
      out.push_back("OUT");
      continue;
    }
    if (in.op == IROp::Mov) {
      out.push_back(LoadOperand(in.arg1));
      out.push_back("STORE " + in.res);
      continue;
    }

    // Binary ops: LOAD arg1 ; OP arg2 ; STORE res
    out.push_back(LoadOperand(in.arg1));
    switch (in.op) {
      case IROp::Add: out.push_back("ADD " + in.arg2); break;
      case IROp::Sub: out.push_back("SUB " + in.arg2); break;
      case IROp::Mul: out.push_back("MUL " + in.arg2); break;
      case IROp::Div: out.push_back("DIV " + in.arg2); break;
      default: break;
    }
    out.push_back("STORE " + in.res);
  }
  return out;
}

}  // namespace edu::compiler

