#include "compiler/Optimizer.h"

namespace edu::compiler {

static bool IsIntLiteral(const std::string& s) {
  if (s.empty()) return false;
  size_t i = 0;
  if (s[0] == '-') i = 1;
  if (i >= s.size()) return false;
  for (; i < s.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
  }
  return true;
}

static edu::Optional<int64_t> ToInt(const std::string& s) {
  if (!IsIntLiteral(s)) return edu::Nullopt;
  try { return std::stoll(s); } catch (...) { return edu::Nullopt; }
}

std::vector<IRInstr> Optimizer::ConstantFolding(const std::vector<IRInstr>& ir) {
  std::vector<IRInstr> out = ir;
  for (auto& in : out) {
    if (in.op == IROp::Add || in.op == IROp::Sub || in.op == IROp::Mul || in.op == IROp::Div) {
      auto a = ToInt(in.arg1);
      auto b = ToInt(in.arg2);
      if (a && b) {
        int64_t r = 0;
        bool ok = true;
        switch (in.op) {
          case IROp::Add: r = *a + *b; break;
          case IROp::Sub: r = *a - *b; break;
          case IROp::Mul: r = (*a) * (*b); break;
          case IROp::Div:
            if (*b == 0) ok = false;
            else r = *a / *b;
            break;
          default: ok = false; break;
        }
        if (ok) {
          in.op = IROp::Mov;
          in.arg1 = std::to_string(r);
          in.arg2.clear();
        }
      }
    }
  }
  return out;
}

std::vector<IRInstr> Optimizer::DeadCodeElim(const std::vector<IRInstr>& ir) {
  // Backwards liveness of variables/temps used.
  std::unordered_set<std::string> live;
  std::vector<bool> keep(ir.size(), true);

  auto markUse = [&](const std::string& v) {
    if (!v.empty() && !IsIntLiteral(v)) live.insert(v);
  };

  for (int i = static_cast<int>(ir.size()) - 1; i >= 0; --i) {
    const auto& in = ir[static_cast<size_t>(i)];
    if (in.HasSideEffect()) {
      markUse(in.arg1);
      continue;
    }
    // If instruction defines res and res not live, and no side effect -> remove
    if (!in.res.empty() && live.find(in.res) == live.end()) {
      keep[static_cast<size_t>(i)] = false;
      continue;
    }
    // Otherwise, res is live, so its args are live
    markUse(in.arg1);
    markUse(in.arg2);
    // Defined variable becomes not-live after definition (classic)
    if (!in.res.empty()) live.erase(in.res);
  }

  std::vector<IRInstr> out;
  out.reserve(ir.size());
  for (size_t i = 0; i < ir.size(); ++i) {
    if (keep[i]) out.push_back(ir[i]);
  }
  return out;
}

std::vector<IRInstr> Optimizer::RemoveRedundant(const std::vector<IRInstr>& ir) {
  std::vector<IRInstr> out;
  out.reserve(ir.size());
  edu::Optional<IRInstr> prev;

  for (const auto& in : ir) {
    // Remove x = x
    if (in.op == IROp::Mov && in.res == in.arg1) continue;

    // Remove immediate duplicates (very simple "redundant instruction" simulation)
    if (prev) {
      const auto& p = *prev;
      if (p.op == in.op && p.res == in.res && p.arg1 == in.arg1 && p.arg2 == in.arg2) {
        continue;
      }
    }
    out.push_back(in);
    prev = in;
  }
  return out;
}

}  // namespace edu::compiler

