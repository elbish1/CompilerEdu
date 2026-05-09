#pragma once

#include "common/Types.h"

namespace edu::tools {

struct StringAnalysis {
  size_t length = 0;
  bool isEven = true;
};

inline StringAnalysis AnalyzeString(const std::string& s) {
  StringAnalysis a;
  a.length = s.size();
  a.isEven = (a.length % 2 == 0);
  return a;
}

}  // namespace edu::tools

