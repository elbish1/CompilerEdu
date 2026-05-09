#pragma once

#include "compiler/Token.h"

namespace edu::compiler {

struct SymbolInfo {
  std::string name;
  std::string type;  // "int"
  bool declared = false;
};

class Lexer {
 public:
  explicit Lexer(std::string input) : input_(std::move(input)) {}

  std::vector<Token> Tokenize();

  const std::unordered_map<std::string, SymbolInfo>& SymbolTable() const { return symbols_; }
  const std::vector<int64_t>& LiteralTable() const { return literals_; }

  static TokenType ClassifyWord(const std::string& word);

 private:
  char Peek(size_t off = 0) const;
  char Get();
  bool Eof() const;

  void SkipWhitespace();
  Token MakeToken(TokenType t, std::string lexeme, SourcePos pos);

  Token LexNumber();
  Token LexIdentifierOrKeyword();

  std::string input_;
  size_t i_ = 0;
  SourcePos pos_{1, 1};

  std::unordered_map<std::string, SymbolInfo> symbols_;
  std::vector<int64_t> literals_;
};

}  // namespace edu::compiler

