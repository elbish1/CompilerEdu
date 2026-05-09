#include "tools/ExpressionEval.h"

namespace edu::tools {

// Shunting-yard to postfix, then evaluate.
static int Prec(char op) {
  switch (op) {
    case '+':
    case '-': return 1;
    case '*':
    case '/': return 2;
    default: return 0;
  }
}

static bool IsOp(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }

EvalResult EvaluateExpression(const std::string& expr) {
  EvalResult r;
  std::vector<std::string> output;
  std::vector<char> ops;

  auto flushOp = [&]() {
    if (!ops.empty()) {
      output.push_back(std::string(1, ops.back()));
      ops.pop_back();
    }
  };

  // Tokenize while converting
  for (size_t i = 0; i < expr.size();) {
    char c = expr[i];
    if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }

    // number (allow unary minus before number)
    if (std::isdigit(static_cast<unsigned char>(c)) ||
        (c == '-' && i + 1 < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i + 1])) &&
         (i == 0 || expr[i - 1] == '(' || IsOp(expr[i - 1])))) {
      size_t j = i;
      if (expr[j] == '-') ++j;
      while (j < expr.size() && std::isdigit(static_cast<unsigned char>(expr[j]))) ++j;
      output.push_back(expr.substr(i, j - i));
      i = j;
      continue;
    }

    if (c == '(') { ops.push_back(c); ++i; continue; }
    if (c == ')') {
      while (!ops.empty() && ops.back() != '(') flushOp();
      if (ops.empty() || ops.back() != '(') {
        r.ok = false;
        r.error = "Mismatched parentheses";
        return r;
      }
      ops.pop_back();  // '('
      ++i;
      continue;
    }

    if (IsOp(c)) {
      while (!ops.empty() && IsOp(ops.back()) && Prec(ops.back()) >= Prec(c)) flushOp();
      ops.push_back(c);
      ++i;
      continue;
    }

    r.ok = false;
    r.error = std::string("Invalid character: '") + c + "'";
    return r;
  }

  while (!ops.empty()) {
    if (ops.back() == '(') {
      r.ok = false;
      r.error = "Mismatched parentheses";
      return r;
    }
    flushOp();
  }

  // Evaluate postfix
  std::vector<int64_t> st;
  for (const auto& tok : output) {
    if (tok.size() == 1 && IsOp(tok[0])) {
      if (st.size() < 2) { r.ok = false; r.error = "Invalid expression"; return r; }
      int64_t b = st.back(); st.pop_back();
      int64_t a = st.back(); st.pop_back();
      int64_t v = 0;
      switch (tok[0]) {
        case '+': v = a + b; break;
        case '-': v = a - b; break;
        case '*': v = a * b; break;
        case '/':
          if (b == 0) { r.ok = false; r.error = "Division by zero"; return r; }
          v = a / b;
          break;
      }
      st.push_back(v);
    } else {
      try { st.push_back(std::stoll(tok)); }
      catch (...) { r.ok = false; r.error = "Invalid number token"; return r; }
    }
  }
  if (st.size() != 1) { r.ok = false; r.error = "Invalid expression"; return r; }
  r.value = st.back();
  return r;
}

}  // namespace edu::tools

