#pragma once

#include "common/Types.h"

namespace edu::tools {

struct EvalResult {
  bool ok = true;
  int64_t value = 0;
  std::string error;
};

// Expression evaluator supporting + - * / and parentheses.
EvalResult EvaluateExpression(const std::string& expr);

}  // namespace edu::tools

