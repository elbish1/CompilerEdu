#pragma once

#include "common/Types.h"

namespace edu::compiler {

enum class TokenType {
  End,
  Unknown,

  // Keywords
  KwInt,
  KwPrint,

  // Identifiers / Literals
  Identifier,
  Number,

  // Operators
  Plus,
  Minus,
  Star,
  Slash,
  Assign,

  // Delimiters
  Semicolon,
  Comma,
  LParen,
  RParen,
};

inline const char* ToString(TokenType t) {
  switch (t) {
    case TokenType::End: return "End";
    case TokenType::Unknown: return "Unknown";
    case TokenType::KwInt: return "Keyword(int)";
    case TokenType::KwPrint: return "Keyword(print)";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Number: return "Number";
    case TokenType::Plus: return "Operator(+)";
    case TokenType::Minus: return "Operator(-)";
    case TokenType::Star: return "Operator(*)";
    case TokenType::Slash: return "Operator(/)";
    case TokenType::Assign: return "Operator(=)";
    case TokenType::Semicolon: return "Delimiter(;)";
    case TokenType::Comma: return "Delimiter(,)";
    case TokenType::LParen: return "Delimiter(()";
    case TokenType::RParen: return "Delimiter())";
  }
  return "Unknown";
}

struct SourcePos {
  int line = 1;
  int col = 1;
};

struct Token {
  TokenType type = TokenType::Unknown;
  std::string lexeme;
  SourcePos pos{};
};

}  // namespace edu::compiler

