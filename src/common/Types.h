#pragma once

#include <cctype>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#if defined(__has_include)
#  if __has_include(<optional>)
#    include <optional>
#    define EDU_HAS_STD_OPTIONAL 1
#  elif __has_include(<experimental/optional>)
#    include <experimental/optional>
#    define EDU_HAS_EXPERIMENTAL_OPTIONAL 1
#  endif
#else
#  include <experimental/optional>
#  define EDU_HAS_EXPERIMENTAL_OPTIONAL 1
#endif
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace edu {

#if defined(EDU_HAS_STD_OPTIONAL)
template <class T>
using Optional = std::optional<T>;
static constexpr auto Nullopt = std::nullopt;
#elif defined(EDU_HAS_EXPERIMENTAL_OPTIONAL)
template <class T>
using Optional = std::experimental::optional<T>;
static constexpr auto Nullopt = std::experimental::nullopt;
#else
#error "No optional implementation available in this toolchain."
#endif

inline std::string Trim(const std::string& s) {
  size_t i = 0;
  while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
  size_t j = s.size();
  while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) --j;
  return s.substr(i, j - i);
}

inline bool IsIdentStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

inline bool IsIdentChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

}  // namespace edu

